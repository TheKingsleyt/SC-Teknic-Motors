# SC-Teknic-Motors
Access the SC controllers, 
In order to run the Code on ROS1. You need Linux OS
2 ways you can accmoplish it:
1. Change your laptop into Linux
2. Download : https://www.virtualbox.org/wiki/Downloads( Download Windows Host)
3. Ubuntu 20.04 or less is compabile to ROS1, Where as Ubuntu 22.04+ compablite to ROS2. The code is in ROS1 so if you want to use ROS2. Change the compatiablity
4. Ubuntu 20.04( Seeing that ROS1 stop updating on May 2025, theirs good chance you need ROS2) https://releases.ubuntu.com/20.04/,
5. Download the desktop image version if you putting in the virutal Box,
6. This if you want to put ROS2 https://ubuntu.com/download/desktop

   Installing Teknic S Foundation SDK on ROS1
   1. Prereq
   [sudo apt update
sudo apt install build-essential cmake git \
    libudev-dev libusb-1.0-0-dev \
    ros-noetic-roscpp ros-noetic-std-msgs]
2. Setup ROS Workspace
mkdir -p ~/teknic_motors/src( you can name it whatever)

cd ~/teknic_motors

catkin_make( amend make if its ROS2, CATKIN MAKE only work for ROS1)
Source the Code
[
echo "source ~/teknic_motors/devel/setup.bash" >> ~/.bashrc
source ~/.bashrc
]
Download S Foundation SK 
[Linux_Software (1).tar.gz](https://github.com/user-attachments/files/22085026/Linux_Software.1.tar.gz)
 Install SDK: 
 
 [
mkdir -p ~/teknic_sdk

cp -r ~/Downloads/sFoundation-*/include ~/teknic_sdk/(Name it whatever just make a new directory

cp -r ~/Downloads/sFoundation-*/libs ~/teknic_sdk/(Name it whatever just make a new directoty)
 ]

Create a ROS Package
1. [cd ~/teknic_motors/src
catkin_create_pkg motor_control roscpp std_msgs
]
2. Edit the given CMakelist
   fix the directory
   path the directory
   and add exceutiable

   Example:


   [include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ~/teknic_sdk/include
)

link_directories(
  ~/teknic_sdk/libs
)

add_executable(sinusoidal_velocity_node src/sinusoidal_velocity_node.cpp)
target_link_libraries(sinusoidal_velocity_node
  ${catkin_LIBRARIES}
  sFoundation20
  pthread
)
]
[another cmake example.docx](https://github.com/user-attachments/files/22085087/another.cmake.example.docx)

Build the Package
cd ~/teknic_motors
catkin_make

Remainder you have to see the path for USB is connected for Motors. 
[ sudo usermod -a -G dialout $USER]
or manaul do it 
<img width="759" height="435" alt="image" src="https://github.com/user-attachments/assets/f06e7970-6820-4125-9301-c2ab238ec9bb" />



