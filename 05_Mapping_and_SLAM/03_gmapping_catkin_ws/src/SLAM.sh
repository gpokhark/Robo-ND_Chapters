#!/bin/sh
source ~/.bashrc
yes
terminator -e "cd ${pwd}/..; source devel/setup.bash; roslaunch turtlebot_gazebo turtlebot_world.launch world_file=worlds/willowgarage.world " &
sleep 5
terminator -e "cd ${pwd}/..; source devel/setup.bash; roslaunch turtlebot_teleop keyboard_teleop.launch" &
sleep 3
terminator -e "cd ${pwd}/..; source devel/setup.bash; rosrun gmapping slam_gmapping" &
sleep 2
terminator -e "cd ${pwd}/..; source devel/setup.bash; rosrun rviz rviz -d ${pwd}/rviz/gmapping.rviz" &