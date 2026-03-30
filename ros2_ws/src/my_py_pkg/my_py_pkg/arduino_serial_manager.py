#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import PoseStamped
import serial, time

class ArduinoSerialManagerNode(Node):
    def __init__(self):
        super().__init__("arduino_serial_manager")
        self.target_sub_ = self.create_subscription(PoseStamped, "goal", self.goal_callback, 10)
        self.x = 0.0
        self.y = 0.0

        # serial line setup
        self.declare_parameter('port', '/dev/ttyACM0') # mega
        self.declare_parameter('baud', 115200)

        port = self.get_parameter('port').get_parameter_value().string_value
        baud = self.get_parameter('baud').get_parameter_value().integer_value

        self.ser = serial.Serial(port, baud, timeout=0.01)
        time.sleep(2) # wait for serial to initialize on arduino

        self.get_logger().info("Arduino Serial Manager Node Initiated")

        self.timer = self.create_timer(0.01, self.read_serial) # check for incoming serial data

    # recieve the goal position
    def goal_callback(self, msg: PoseStamped):
        self.x = msg.pose.position.x
        self.y = msg.pose.position.y
        self.send_serial()

    def send_serial(self):
        # send the x and y position as a comma separated string over serial to the arduino
        serial_data = f"{self.x},{self.y}\n"
        self.ser.write(serial_data.encode('utf-8'))

    def read_serial(self):
        # check if there is serial dat waiting
        if self.ser.in_waiting == 0:
            return

        # get serial data in the line
        line = self.ser.readline().decode('utf-8', errors='ignore').strip()
        if not line:
            return

        # split the serial data coming in as a string into individual floats
        parts = line.split(',')
        if len(parts) == 2:
            test_x = float(parts[0].strip())
            test_y = float(parts[1].strip())
            self.get_logger().info(f"Received serial data: x={test_x}, y={test_y}")
        else:
            self.get_logger().warn(f"Failed to parse data: {line}")



def main(args=None):
    rclpy.init(args=args)
    node = ArduinoSerialManagerNode()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == "__main__":
    main()