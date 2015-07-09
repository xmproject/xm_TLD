#include "ros/ros.h"
#include "xm_TLD/array.h"
#include <cstdlib>
#include <iostream>
using namespace std;
int main(int argc, char **argv)
{
  ros::init(argc, argv, "give");
  
  ros::NodeHandle n;
  ros::ServiceClient client = n.serviceClient<xm_TLD::array>("serve");
  xm_TLD::array  srv;
  srv.request.a = 1;
  
  if (client.call(srv))
  {
    	int len=srv.response.array.tracks.size();
	for(int i=0;i<len;i++)
		cout<<srv.response.array.tracks[i]<<endl;
  }
  

  return 0;
}

