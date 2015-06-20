#include <Strategy.h>

using namespace std;
int flag=0;
int id;

//opt_msgs::Track tracks[];

int main(int argc, char * argv[])
{
	ros::init(argc, argv, "control");
	ros::NodeHandle main;
	ros::Rate waiting_rate(30);
	
	if(flag==0)
	{
		openptrack ptrack;
		while(ptrack.file==false)
		{
			ros::spinOnce();
        		waiting_rate.sleep();
		}
		id=ptrack.id;
		flag=1;
		
	}
	while(1)
	{
	control control(id);
	while(control.file==false)
	{
		
		ros::spinOnce();
        	waiting_rate.sleep();
	}
	id=control.id;
	cout<<"id="<<id<<endl;	
	}
	return 0;
}
	
