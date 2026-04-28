#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import PoseStamped, Twist
import math
import tf2_ros
from sensor_msgs.msg import Imu
import tf2_geometry_msgs


class LocalPlannerTesterNode(Node):
    def __init__(self):
        super().__init__("local_planner_tester")
        self.target_sub_ = self.create_subscription(PoseStamped, "goal", self.goal_callback, 10) # recieves at about 10hz based on the UWB recieving
        self.cmd_pub_ = self.create_publisher(Twist, "cmd_vel", 10)

        self.imu_sub = self.create_subscription(Imu, "/imu/data", self.imu_callback, 10)

        # initialize the transform tree
        self.tf_buffer = tf2_ros.Buffer()
        self.tf_listener = tf2_ros.TransformListener(self.tf_buffer, self)

        # factors for linear control
        self.k_lin = 0.6
        # factors for PD smoothing
        self.kp = 0.5
        self.kd = 0.01
        # constraints
        self.max_lin = 0.56
        self.max_ang = 0.8

        self.max_to_the_side = 1.0

        self.follow_distance = 0.5

        # imut
        self.imu_gyro_z = 0.0
        self.last_imu_time = None
        self.imu_timeout = 0.2  # seconds to wait before giving up on IMU


        self.get_logger().info("Local Planner Tester Node Initiated")

    def imu_callback(self, msg: Imu):
        raw_imu_gyro_z = msg.angular_velocity.z
        # EWMA to smooth the IMU input
        alpha = 0.5
        self.imu_gyro_z = alpha * raw_imu_gyro_z + (1.0 - alpha) * self.imu_gyro_z
        self.last_imu_time = self.get_clock().now()

    # send the cmd_vel message every time the goal is updated
    def goal_callback(self, msg: PoseStamped):
        if self.tf_buffer.can_transform("base_link", msg.header.frame_id, rclpy.time.Time()):
            goal_transform_to_base = self.tf_buffer.transform(msg, "base_link")
        else:
            self.get_logger().info("Failed to get tf for uwb")
            return

        x = goal_transform_to_base.pose.position.x
        y = goal_transform_to_base.pose.position.y

        # recieve the x,y back as the angular z and linear x
        linear_x, angular_z = self.compute_cmd(x, y)

        # publish the cmd_vel message
        vel_msg = Twist()
        vel_msg.linear.x = float(linear_x)
        vel_msg.angular.z = float(angular_z)
        self.cmd_pub_.publish(vel_msg)

    def compute_cmd(self, x, y):
        distance = math.sqrt(x*x + y*y)
        angle_error = math.atan2(y, x)

        # set follow distance
        distance_error = distance - self.follow_distance

        # check for last IMU update and set to zero if no recent update
        imu_z = 0.0
        now = self.get_clock().now()
        if self.last_imu_time is not None:
            delta_imu = (now - self.last_imu_time).nanoseconds / 1e9
            if delta_imu < self.imu_timeout:
                imu_z = self.imu_gyro_z

        # PD heading correction
        angular_z = self.kp * angle_error - self.kd * imu_z
        linear_x = self.k_lin * distance_error

        # clamp outputs
        angular_z = max(min(angular_z, self.max_ang), -self.max_ang)
        linear_x = max(min(linear_x, self.max_lin), 0.0)


        # Increase to above deadzone
        if angular_z > 0 and angular_z < 0.17 and distance_error > 0:
            angular_z = 0.17
        if angular_z < 0 and angular_z > -0.17 and distance_error > 0:
            angular_z = -0.17
        if linear_x > 0 and linear_x < 0.17 and distance_error > 0:
            linear_x = 0.17
        if distance_error < 0:
            angular_z = 0

        # stop moving forward if target is too far off to the side
        if abs(angle_error) > self.max_to_the_side:
            linear_x = 0.0
            angular_z = angular_z - 0.5 if angular_z < 0 else angular_z + 0.5

        return linear_x, angular_z

def main(args=None):
    rclpy.init(args=args)
    node = LocalPlannerTesterNode()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == "__main__":
    main()