#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from trolley_interfaces.msg import UwbData
import serial

class UwbDriverNode(Node):
    def __init__(self):
        super().__init__('uwb_driver_node')

        self.declare_parameter('port', '/dev/serial/by-id/usb-Silicon_Labs_CP2104_USB_to_UART_Bridge_Controller_02C552FA-if00-port0') # anchor 3
        self.declare_parameter('baud', 115200)

        port = self.get_parameter('port').get_parameter_value().string_value
        baud = self.get_parameter('baud').get_parameter_value().integer_value

        self.uwbDataPublisher = self.create_publisher(UwbData, "uwb/data", 10)

        self.ser = serial.Serial(port, baud, timeout=0.1)
        self.get_logger().info(f"Reading UWB serial from {port} @ {baud}, publishing to /uwb/data")

        self.timer = self.create_timer(0.01, self.read_serial)

    def read_serial(self):
        try:
            line = self.ser.readline().decode('utf-8', errors='ignore').strip()
            if not line:
                return

            msg = UwbData()
            msg.raw_distance = line # parse this using the comma
            # Example line format: "3, 22.1" where 3 is anchor_id and 22.1 is distance
            # parts = line.split(',')
            # if len(parts) == 2:
            #     msg.distance = float(parts[0].strip())
            #     msg.anchor_id = int(parts[1].strip())
            # else:
            #     self.get_logger().warn(f"Failed to parse UWB data: {line}")

            self.uwbDataPublisher.publish(msg)
        except Exception as e:
            self.get_logger().warn(f"Serial read error: {e}")


def main(args=None):
    rclpy.init(args=args)
    node = UwbDriverNode()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == "__main__":
    main()