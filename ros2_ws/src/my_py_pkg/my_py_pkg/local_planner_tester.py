#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import PoseStamped


class LocalPlannerTesterNode(Node):
    def __init__(self):
        super().__init__("local_planner_tester")
        self.target_sub_ = self.create_subscription(PoseStamped, "goal", self.goal_callback, 10)
        self.get_logger().info("Local Planner Tester Node Initiated")

    def goal_callback(self, msg: PoseStamped):
        self.get_logger().info(f"Received goal: x={msg.pose.position.x}, y={msg.pose.position.y}, z={msg.pose.position.z}")

def main(args=None):
    rclpy.init(args=args)
    node = LocalPlannerTesterNode()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == "__main__":
    main()