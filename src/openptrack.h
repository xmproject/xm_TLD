#ifndef OPENPTRACK
#define OPENPTRACK

#include <ros/ros.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opt_msgs/TrackArray.h>
#include <std_msgs/String.h>
#include "xm_TLD/pubdata.h"
#include <std_msgs/Int32.h>

class openptrack
{
public:
	openptrack();
	~openptrack();
	ros::NodeHandle main;
	ros::Subscriber getdata;
	ros::Publisher give;
	ros::ServiceClient client;
	//std_msgs::String stringdata;
	int id;
	void chatterCallback(const opt_msgs::TrackArray& msg);
	bool file;
};

#endif	
