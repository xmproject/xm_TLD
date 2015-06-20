#include <ros/ros.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opt_msgs/TrackArray.h>
#include <std_msgs/String.h>
#include <openptrack.h>
#include <opt_msgs/Track.h>
#include <people_msgs/Person.h>
#include <cmath>

using namespace std;


openptrack::openptrack()
{
	
        getdata=main.subscribe("/tracker/tracks_smoothed",1,&openptrack::chatterCallback,this);
	file=false;

}
openptrack::~openptrack()
{
}
void openptrack::chatterCallback(const opt_msgs::TrackArray& msg)
{
        //people_msgs::Person[] people;
	//opt_msgs::Track *track=msg;
        
	int length=msg.tracks.size();
	cout<<length<<endl;
	if(length==0)
		return;
      	float k=10000000.0,longl;
	int i=0,mm=0,x,y,width,height;
	for(i=0;i<length;i++)
	{
		
   		longl=fabs(msg.tracks[i].box_2D.x-319.5)+fabs(msg.tracks[i].box_2D.x+msg.tracks[i].box_2D.width-319.5);
		cout<<longl<<endl;
		if(longl<k &&msg.tracks[i].distance < 4)
                { mm=i;k=longl;}
	}
        
        x=msg.tracks[mm].box_2D.x;
	y=msg.tracks[mm].box_2D.y;
	width=msg.tracks[mm].box_2D.width;
	height=msg.tracks[mm].box_2D.height;
	
	std_msgs::String massg;
	std::stringstream ss;
	ss<<x<<","<<y<<","<<width<<","<<height;
	massg.data=ss.str();
	//stringdata=massg;		
   	ROS_INFO("%s", massg.data.c_str());
	while(1)
	{
		client = main.serviceClient<follower::pubdata>("givedata");
		follower::pubdata srv;
		srv.request.a=massg.data;
		if (client.call(srv))
  		{
    			id=msg.tracks[mm].id;
			file=true;
			break;
  		}
	}
	

}


