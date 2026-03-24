# Tools and commands used in development

## Tmux
### Open Tmux
Start a session
```
tmux
```
use this further command to reaattach to this same session later. First create it:
```
tmux new -s <name>
```
Then reattach to it:
```
tmxu attach -s <name>
```
Show all active session with:
```
tmux ls
```
### Pane management
`Ctrl + B` must preface any command to do work in tmux.
- `"` - Split horizontally
- `%` - Split vertically
- arrow keys - move panes
- `exit` or `Ctrl + D` - close one window at a time
-  `tmux kill-session -t <name>` - end entire thing at once
- `d` - detach but leave session running in background

## Pi
### Helpful commands
To end and leave pi use
```
sudo poweroff
```
Built a swapfile. Check its use using
```
free -h
```
To change the wifi run this command to get the config file and then apply it
```
sudo nano /etc/netplan/50-cloud-init.yaml
sudo netplan apply
```
Then check if the wifi is connected using `ip a`

## Lidar
To launch file for lidar run
```
ros2 launch ldlidar_stl_ros2 ld19.launch.py
```
