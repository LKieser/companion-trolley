#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from std_msgs.msg import Header, String
from sensor_msgs.msg import Imu
from geometry_msgs.msg import Twist
import serial, time
import re

class ArduinoSerialManagerNode(Node):
    def __init__(self):
        super().__init__("arduino_serial_manager")

        # serial line setup
        self.ser = serial.Serial('/dev/ttyACM0', 115200, timeout=0.01)
        time.sleep(2) # wait for serial to initialize on arduino
        self.timer = self.create_timer(0.01, self.read_serial) # check for incoming serial data

        # publisher for IMU
        self.imu_pub_ = self.create_publisher(Imu, '/imu/data', 10)
        self.bad_msg = False

        # publisher for VERBOSE
        self.verbose_pub_ = self.create_publisher(String, '/verbose', 10)

        # subscriber to cmd_vel
        self.cmd_vel_sub_ = self.create_subscription(Twist, '/cmd_vel', self.send_cmd_vel_callback, 10)

        self.get_logger().info("Arduino Serial Manager Node Initiated")

    # example sending serial mode
    def send_cmd_vel_callback(self, vel_msg: Twist):
        linear_x = vel_msg.linear.x
        angular_z = vel_msg.angular.z
        # send the x and y position as a comma separated string over serial to the arduino
        serial_data = f"CMD_VEL, {linear_x},{angular_z}\n"
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

        # Check first word for type of data and operate accordingly
        # Print debug immediatly if something happens
        if parts[0].strip() == "DEBUG":
            for items in parts[1:]:
                self.get_logger().info(items)
        # print extra helpful verbose data into a topic
        elif parts[0].strip() == "VERBOSE":
            pwm_string = ','.join(parts[1:])
            verb_msg = String()
            verb_msg.data = f"PWM: LEFT, RIGHT: {pwm_string}"
            verb_msg.data
            self.verbose_pub_.publish(verb_msg)
        elif parts[0].strip() == "IMU":
            if len(parts[1:]) == 10:     # 10 is the expected number of parts needed from the IMU
                self.publish_imu_data(parts[1:])
            else:
                self.get_logger().warn(f"Not all fields filled in IMU line: {line}")
        else:
            self.get_logger().warn(f"Failed to parse data: {line}")

    def safe_float(self, value):
        value = value.strip()

        # keep only digits, decimal point, minus sign, plus sign, exponent chars
        value = re.sub(r'[^0-9\+\-\.]', '', value)

        # valid float pattern
        if not re.fullmatch(r'[+-]?(\d+(\.\d*)?|\.\d+)', value):
            self.bad_msg = True

        return float(value)

    def publish_imu_data(self, imu_data):
        if len(imu_data) < 10:
            self.get_logger().warn(f"IMU data too short: {imu_data}")
            return

        msg = Imu()

        msg.header = Header()
        msg.header.stamp = self.get_clock().now().to_msg()
        msg.header.frame_id = 'imu_link'

        # Orientation (quaternion)
        msg.orientation.w = self.safe_float(imu_data[0].strip())
        msg.orientation.x = self.safe_float(imu_data[1].strip())
        msg.orientation.y = self.safe_float(imu_data[2].strip())
        msg.orientation.z = self.safe_float(imu_data[3].strip())

        # Angular velocity (rad/s)
        msg.angular_velocity.x = self.safe_float(imu_data[4].strip())
        msg.angular_velocity.y = self.safe_float(imu_data[5].strip())
        msg.angular_velocity.z = self.safe_float(imu_data[6].strip())

        # Linear acceleration (m/s^2)
        msg.linear_acceleration.x = self.safe_float(imu_data[7].strip())
        msg.linear_acceleration.y = self.safe_float(imu_data[8].strip())
        msg.linear_acceleration.z = self.safe_float(imu_data[9].strip())

        if not self.bad_msg:
            self.imu_pub_.publish(msg)
        else:
            self.get_logger().warn("Bad IMU packet, skipping")
            self.bad_msg = False



def main(args=None):
    rclpy.init(args=args)
    node = ArduinoSerialManagerNode()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == "__main__":
    main()