#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from trolley_interfaces.msg import UwbData
from geometry_msgs.msg import PoseStamped

class TargetFollowerNode(Node):
    def __init__(self):
        super().__init__("target_follower")
        self.smoothed_x = 0
        self.smoothed_y = 0
        self.sub_ = self.create_subscription(UwbData, "uwb/data", self.uwb_callback, 10)
        self.pub_ = self.create_publisher(PoseStamped, "goal", 10)
        self.get_logger().info("Target Follower Node Initiated")

    # Process the received UWB data, apply smoothing, and publish the smoothed position as a PoseStamped message
    def uwb_callback(self, msg: UwbData):
        # Use EWMA function to smooth the UWB data
        if msg.rmse > 1.0:  # If the RMSE is too high, ignore the data
            # self.get_logger().warn(f"High RMSE ({msg.rmse}), ignoring UWB data")
            return

        alpha = 0.1 # smoothing factor
        self.smoothed_x = alpha * msg.x + (1 - alpha) * self.smoothed_x
        self.smoothed_y = alpha * msg.y + (1 - alpha) * self.smoothed_y

        # apply heavier smoothing for testing


        # publish smoothed data
        pose_msg = PoseStamped()
        pose_msg.header.stamp = self.get_clock().now().to_msg()
        pose_msg.header.frame_id = "map"

        pose_msg.pose.position.x = self.smoothed_x
        pose_msg.pose.position.y = self.smoothed_y
        pose_msg.pose.position.z = 0.0

        # add pose for rviz
        pose_msg.pose.orientation.x = 0.0
        pose_msg.pose.orientation.y = 0.0
        pose_msg.pose.orientation.z = 0.0
        pose_msg.pose.orientation.w = 1.0

        self.pub_.publish(pose_msg)



def main(args=None):
    rclpy.init(args=args)
    node = TargetFollowerNode()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == "__main__":
    main()