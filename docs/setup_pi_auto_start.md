# Use systemct1 to auto launch ros2
On base operating system run
```
nano ~/start_trolley.sh
```
Inside this new file paste the following code
```
#!/bin/bash
source /opt/ros/jazzy/setup.bash
source /home/luke/companion_trolley/ros2_ws/install/setup.bash

export ROS_DOMAIN_ID=5
export RMW_IMPLEMENTATION=rmw_cyclonedds_cpp
export ROS_AUTOMATIC_DISCOVERY_RANGE=LOCALHOST

ros2 launch companion_trolley_bringup bringup.launch.xml
```
Then make it executable
```
chmod +x ~/start_trolley.sh
```
Create the systemd service
```
sudo nano /etc/systemd/system/trolley.service
```
Inside this file paste the following code
```
[Unit]
Description=Companion Trolley ROS 2 Bringup
After=multi-user.target systemd-udev-settle.service
Wants=systemd-udev-settle.service

[Service]
Type=simple
User=luke
Group=luke
SupplementaryGroups=dialout
WorkingDirectory=/home/luke/companion_trolley/ros2_ws
Environment=HOME=/home/luke
Environment=USER=luke
Environment=ROS_DOMAIN_ID=5
Environment=RMW_IMPLEMENTATION=rmw_cyclonedds_cpp
Environment=ROS_AUTOMATIC_DISCOVERY_RANGE=LOCALHOST
ExecStartPre=/bin/sleep 45
ExecStart=/home/luke/start_trolley.sh
Restart=always
RestartSec=5

[Install]
WantedBy=multi-user.target
```
Then run the following three commands to initialize it at boot
```
sudo systemctl daemon-reload
sudo systemctl enable trolley.service
sudo systemctl start trolley.service
```