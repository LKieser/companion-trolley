#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist


class CmdVelSmoothedForwardNode(Node):
    def __init__(self):
        super().__init__("cmd_vel_smoothed_forward")

        self.cmd_vel_smoothed_sub_ = self.create_subscription(Twist, '/cmd_vel_smoothed', self.forward_cmd_vel_callback, 10)
        self.cmd_vel_smoothed_pub_ = self.create_publisher(Twist, '/cmd_vel', 10)

    def forward_cmd_vel_callback(self, msg: Twist):
        self.cmd_vel_smoothed_pub_.publish(msg)


def main(args=None):
    rclpy.init(args=args)
    node = CmdVelSmoothedForwardNode()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == "__main__":
    main()