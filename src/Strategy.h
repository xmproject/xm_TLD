#ifndef CONTROL
#define CONTROL

#include <ros/ros.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opt_msgs/TrackArray.h>
#include <std_msgs/String.h>
#include <openptrack.h>
#include "xm_TLD/num.h"
#include <people_msgs/People.h>
#include <people_msgs/PositionMeasurement.h>
#include <people_msgs/PositionMeasurementArray.h>
#include <cmath>
#include "xm_TLD/box.h"
class control
{
public:
	control(int s);
	bool file;
	bool tldfile;
	bool legfile;
	ros::NodeHandle m;
	ros::Publisher givedata;
	ros::Publisher boxdata;
	ros::Subscriber tlddata;
	ros::Subscriber legdata;
	int id;
	ros::Subscriber track;
	opt_msgs::TrackArray  swap;
	void tldCallback(const xm_TLD::num& msg);
	void Callback(const opt_msgs::TrackArray& msg);
	void legCallback(const people_msgs::PositionMeasurementArray& msg);
	
};

#endif

