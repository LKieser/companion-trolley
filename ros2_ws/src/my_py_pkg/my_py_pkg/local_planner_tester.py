#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import PoseStamped


class LocalPlannerTesterNode(Node):
    def __init__(self):
        super().__init__("local_planner_tester")
        self.target_sub_ = self.create_subscription(PoseStamped, "goal", self.goal_callback, 10)
        self.cmd_pub_ = self.create_publisher(PoseStamped, "cmd_vel", 10)
        self.get_logger().info("Local Planner Tester Node Initiated")

    # send the cmd_vel message every time the goal is updated
    def goal_callback(self, msg: PoseStamped):
        send_msg = PoseStamped()
        send_msg.pose.position.x = msg.pose.position.x
        send_msg.pose.position.y = msg.pose.position.y
        send_msg.pose.position.z = 0.0
        self.cmd_pub_.publish(send_msg)

def main(args=None):
    rclpy.init(args=args)
    node = LocalPlannerTesterNode()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == "__main__":
    main()