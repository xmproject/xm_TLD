#include <ros/ros.h>
#include <iostream>

#include "follower/array.h"
#include <opt_msgs/TrackArray.h>

using namespace std;

opt_msgs::TrackArray  data;

void chatterCallback(const opt_msgs::TrackArray& msg)
{
	data=msg;
		
}	
bool response(follower::array::Request &req,follower::array::Response &res)
{
	
	res.array=data;
 
	return true;
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "whoiswho");
        ros::NodeHandle n;
        //ros::Rate waiting_rate(30);
	ros::Subscriber sub = n.subscribe("/tracker/tracks_smoothed", 1 , chatterCallback);
	
	ros::ServiceServer service = n.advertiseService("serve", response);
	ros::spin();
	return 0;
}
	
	
