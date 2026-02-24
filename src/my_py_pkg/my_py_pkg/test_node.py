#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from trolley_interfaces.msg import UwbData


class TestNode(Node):
    def __init__(self):
        super().__init__("test_node")
        self.pub = self.create_publisher(UwbData, "test_topic", 10)
        self.timer = self.create_timer(1.0, self.publish_credentials)
        self.get_logger().info("Test Node Initiated")

    def publish_credentials(self):
        msg = UwbData()
        msg.name = "Luke"
        msg.age = 22
        self.pub.publish(msg)


def main(args=None):
    rclpy.init(args=args)
    node = TestNode()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == "__main__":
    main()