#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from trolley_interfaces.msg import UwbData
import serial

class UwbDriverNode(Node):
    def __init__(self):
        super().__init__('uwb_driver_node')

        self.declare_parameter('port', '/dev/ttyAMA0') # anchor 1
        self.declare_parameter('baud', 115200)

        port = self.get_parameter('port').get_parameter_value().string_value
        baud = self.get_parameter('baud').get_parameter_value().integer_value

        self.uwbDataPublisher = self.create_publisher(UwbData, "uwb/data", 10)

        self.ser = serial.Serial(port, baud, timeout=0.1)
        self.get_logger().info(f"Reading UWB serial from {port} @ {baud}, publishing to /uwb/data")

        self.timer = self.create_timer(0.01, self.read_serial)

    def read_serial(self):
        line = self.ser.readline().decode('utf-8', errors='ignore').strip()
        if not line:
            return

        # split the serial data coming in as a string into individual floats
        msg = UwbData()
        parts = line.split(',')
        if len(parts) == 3:
            msg.x = float(parts[0].strip())
            msg.y = float(parts[1].strip())
            msg.rmse = float(parts[2].strip())
        else:
            self.get_logger().warn(f"Failed to parse UWB data: {line}")

        self.uwbDataPublisher.publish(msg)


def main(args=None):
    rclpy.init(args=args)
    node = UwbDriverNode()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == "__main__":
    main()