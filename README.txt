# xm_TLD

Create Date: 2015.6.17

Authors: flame_of_youth  

Function: this package uses TLD and openPtrack to get the position of tracking target.
if you want to run this package,you must first run the openPtrack and openni.

download the openPtrack pacakge and run:
roslaunch tracking detection_and_tracking.launch

when you have run openni,you must true the depth_registration.

now you can use this package to run follow commands:
rosrun xm_TLD follow_TLD
rosrun xm_TLD control
