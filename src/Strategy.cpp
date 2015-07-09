#include <Strategy.h>

using namespace std;

control::control(int s)
{
	id=s;
	givedata=m.advertise<people_msgs::PositionMeasurement>("people_position_estimation",1);
	boxdata=m.advertise<xm_TLD::box>("boxdata",1);
	track=m.subscribe("/tracker/tracks_smoothed",1,&control::Callback,this);
	tlddata=m.subscribe("tld_people_position_estimation",1,&control::tldCallback,this);
	legdata=m.subscribe("people_tracker_measurements",1,&control::legCallback,this);
	file=false;
	tldfile=false;
	legfile=false;
}
void control::tldCallback(const xm_TLD::num& msg)
{
	//int i;
	/*if(msg.a==0)
	{
		data.people[0].name="";
		data.people[0].position.x=0;
		data.people[0].position.y=0;
		data.people[0].position.z=0;
		data.people[0].reliability=0.0;
		givedata.publish(data);
	}*/
	
	if(msg.a==1&&tldfile==true)
	{
		
		people_msgs::PositionMeasurement data;
		data.header=swap.header;
		data.name="1";
		data.pos.x=msg.x;
		data.pos.y=msg.y;
		givedata.publish(data);
		int preid=-1,length1=swap.tracks.size(),ii;
		//cout<<"length1="<<length1<<endl;

		float maxx=100000.0;
		for(ii=0;ii<length1&&swap.tracks[ii].id>=id;ii++)
		{
			float dis=sqrt((swap.tracks[ii].x-msg.x)*(swap.tracks[ii].x-msg.x)+(swap.tracks[ii].y-msg.y)*(swap.tracks[ii].y-msg.y));
			if(dis<maxx)
			{preid=ii;maxx=dis;}
		}
		if(preid!=-1&&maxx<0.5)
		{id=swap.tracks[preid].id;
			cout<<"new id="<<id<<endl;
		file=true;}
		
	
	}
	/*else if(tldfile==true)
	{
		int length1=swap.tracks.size(),i,mm=-1;
		float k=10000000.0,longl;
		for(i=0;i<length1;i++)
		{
		
   			longl=fabs(swap.tracks[i].box_2D.x-319.5)+fabs(swap.tracks[i].box_2D.x+swap.tracks[i].box_2D.width-319.5);
			//cout<<longl<<endl;
			if(longl<k)
                	{ mm=i;k=longl;}
		}
		if(mm!=-1)
		{
			id=swap.tracks[mm].id;
			cout<<"new id="<<id<<endl;
		}
		file=true;
	}*/
	else
		legfile=true;
		
	
}
void control::legCallback(const people_msgs::PositionMeasurementArray& msg)
{
	if(legfile==true&&msg.people.size()!=0)
	{
		int length1=swap.tracks.size(),ii,mm=-1,preid=-1;
		float maxx=10000000.0,longl;
		for(ii=0;ii<length1&&swap.tracks[ii].id>=id;ii++)
		{
			float dis=sqrt((swap.tracks[ii].x-msg.people[0].pos.x)*(swap.tracks[ii].x-msg.people[0].pos.x)+(swap.tracks[ii].y-msg.people[0].pos.y)*(swap.tracks[ii].y-msg.people[0].pos.y));
			if(dis<maxx)
			{preid=ii;maxx=dis;}
		}
		if(preid!=-1&&maxx<0.5)
		{id=swap.tracks[preid].id;
			cout<<"new id="<<id<<endl;}
		file=true;
	
	}
}
void control::Callback(const opt_msgs::TrackArray& msg)
{
	
	
 	int i,k=-1,s;
	int length=msg.tracks.size();
	cout<<"length="<<length<<endl;
 
	for(i=0;i<length;i++)
	{
		if(msg.tracks[i].id==id)
			{k=i;break;}
	}
	cout<<"k="<<k<<endl;
	
	for(s=0;s<length;)
	{
		people_msgs::PositionMeasurement data;
		data.header=msg.header;
		if(s==k)
			{s++;continue;}
		data.name="";
		data.pos.x=msg.tracks[s].x;
		data.pos.y=msg.tracks[s].y;
		givedata.publish(data);
		//data.people[i].reliability=float(msg.tracks[s].id);
		s++;
	}
		
	if(k!=-1)
	{
		
		people_msgs::PositionMeasurement data;
		data.header=msg.header;
		data.name="1";
		data.pos.x=msg.tracks[k].x;
		data.pos.y=msg.tracks[k].y;
		//data.people[0].position.z=msg.tracks[k].distance;
		//data.people[0].reliability=float(msg.tracks[k].id);
		givedata.publish(data);
		xm_TLD::box box;
		box.x=msg.tracks[k].box_2D.x;
		box.y=msg.tracks[k].box_2D.y;
		box.width=msg.tracks[k].box_2D.width;
		box.height=msg.tracks[k].box_2D.height;
		boxdata.publish(box);
		file=true;
	}
		

	else 
	{
		swap=msg;
		tldfile=true;
		xm_TLD::box box;
		box.x=0;
		box.y=0;
		box.width=0;
		box.height=0;
		boxdata.publish(box);
		
	}
}



