#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from std_msgs.msg import Header
from sensor_msgs.msg import Imu
import serial, time

class ArduinoSerialManagerNode(Node):
    def __init__(self):
        super().__init__("arduino_serial_manager")

        # serial line setup
        self.ser = serial.Serial('/dev/ttyACM0', 115200, timeout=0.01)
        time.sleep(2) # wait for serial to initialize on arduino
        self.timer = self.create_timer(0.01, self.read_serial) # check for incoming serial data

        # publisher for IMU
        self.imu_pub_ = self.create_publisher(Imu, '/imu/data', 10)

        self.get_logger().info("Arduino Serial Manager Node Initiated")

    # example sending serial mode
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

        # Check first word for type of data and operate accordingly
        if parts[0].strip() == "DEBUG":
            for items in parts[1:]:
                self.get_logger().info(items)
        elif parts[0].strip() == "IMU":
            if len(parts[1:]) == 10:     # 10 is the expected number of parts needed from the IMU
                self.publish_imu_data(parts[1:])
            else:
                self.get_logger().warn(f"Not all field filled in IMU line: {line}")
        else:
            self.get_logger().warn(f"Failed to parse data: {line}")

    def publish_imu_data(self, imu_data):
        if len(imu_data) < 10:
            self.get_logger().warn(f"IMU data too short: {imu_data}")
            return

        msg = Imu()

        msg.header = Header()
        msg.header.stamp = self.get_clock().now().to_msg()
        msg.header.frame_id = 'imu_link'

        # Orientation (quaternion)
        msg.orientation.w = float(imu_data[0].strip())
        msg.orientation.x = float(imu_data[1].strip())
        msg.orientation.y = float(imu_data[2].strip())
        msg.orientation.z = float(imu_data[3].strip())

        # Angular velocity (rad/s)
        msg.angular_velocity.x = float(imu_data[4].strip())
        msg.angular_velocity.y = float(imu_data[5].strip())
        msg.angular_velocity.z = float(imu_data[6].strip())

        # Linear acceleration (m/s^2)
        msg.linear_acceleration.x = float(imu_data[7].strip())
        msg.linear_acceleration.y = float(imu_data[8].strip())
        msg.linear_acceleration.z = float(imu_data[9].strip())

        self.imu_pub_.publish(msg)


def main(args=None):
    rclpy.init(args=args)
    node = ArduinoSerialManagerNode()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == "__main__":
    main()