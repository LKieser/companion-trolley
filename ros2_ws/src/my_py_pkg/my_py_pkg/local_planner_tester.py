#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import PoseStamped, Twist
import math


class LocalPlannerTesterNode(Node):
    def __init__(self):
        super().__init__("local_planner_tester")
        self.target_sub_ = self.create_subscription(PoseStamped, "goal", self.goal_callback, 10) # recieves at about 10hz
        self.cmd_pub_ = self.create_publisher(Twist, "cmd_vel", 10)
        self.get_logger().info("Local Planner Tester Node Initiated")

    # send the cmd_vel message every time the goal is updated
    def goal_callback(self, msg: PoseStamped):
        # recieve the x,y back as the angular z and linear x
        temp_x, temp_y = self.compute_cmd(msg.pose.position.x, msg.pose.position.y)

        # publish the cmd_vel message
        vel_msg = Twist()
        vel_msg.linear.x = float(temp_x)
        vel_msg.angular.z = float(temp_y)
        self.cmd_pub_.publish(vel_msg)

    def compute_cmd(self, x, y):
        distance = math.sqrt(x*x + y*y)
        angle_error = math.atan2(y, x)

        # set follow distance
        follow_distance = 1.0
        distance_error = distance - follow_distance

        # self.get_logger().info(f"distance error: {distance_error}")

        # factors for proportional control
        k_lin = 0.6
        k_ang = 1 # not using this for now
        max_lin = 0.56 # 0.56
        max_ang = 0.8 # 0.8

        angular_z = k_ang * angle_error
        linear_x = k_lin * distance_error

        # stop moving forward if target is too far off to the side
        # if abs(angle_error) > 0.5:
        #     linear_x = 0.0

        # clamp outputs
        angular_z = max(min(angular_z, max_ang), -max_ang)
        linear_x = max(min(linear_x, max_lin), 0.0)

        # Increase to above deadzone
        if angular_z > 0 and angular_z < 0.17 and distance_error > 0:
            angular_z = 0.17
        if angular_z < 0 and angular_z > -0.17 and distance_error > 0:
            angular_z = -0.17
        if linear_x > 0 and linear_x < 0.17 and distance_error > 0:
            linear_x = 0.17
        if distance_error < 0:
            angular_z = 0

        return linear_x, angular_z

def main(args=None):
    rclpy.init(args=args)
    node = LocalPlannerTesterNode()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == "__main__":
    main()