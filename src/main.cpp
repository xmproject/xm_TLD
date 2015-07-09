#include <opencv2/opencv.hpp>
#include <tld_utils.h>
#include <iostream>
#include <sstream>
#include <TLD.h>
#include <stdio.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Vector3.h>
#include <std_msgs/String.h>
#include "reading_image.h"
#include "sub.h"
#include "xm_TLD/num.h"
#include "xm_TLD/pubdata.h"
using namespace cv;
using namespace std;
//Global variables
Rect box;
bool drawing_box = false;
bool gotBB = false;
bool tl = true;
bool rep = false;
bool fromfile=false;
//bool file=false;
string video;
bool vis=false;

//bounding box mouse callback
void mouseHandler(int event, int x, int y, int flags, void *param){
  switch( event ){
  case CV_EVENT_MOUSEMOVE:
    if (drawing_box){
        box.width = x-box.x;
        box.height = y-box.y;
    }
    break;
  case CV_EVENT_LBUTTONDOWN:
    drawing_box = true;
    box = Rect( x, y, 0, 0 );
    break;
  case CV_EVENT_LBUTTONUP:
    drawing_box = false;
    if( box.width < 0 ){
        box.x += box.width;
        box.width *= -1;
    }
    if( box.height < 0 ){
        box.y += box.height;
        box.height *= -1;
    }
    gotBB = true;
    break;
  }
}

void print_help(char** argv){
  printf("use:\n     %s -p /path/parameters.yml\n",argv[0]);
  printf("-s    source video\n-b        bounding box file\n-tl  track and learn\n-r     repeat\n");
}



double average_depth(Mat& depth, BoundingBox box)
{
    //cout<<"channels:"<<depth.channels()<<endl;
    if (!depth.data)
    {
        cout<<"no data!"<<endl;
        return 0.0;
    }
    float sum=0.0;
    int n = 0;
    //cout<<"in depth loop"<<endl;
    for( int nrow = box.x; nrow<box.x+box.width; nrow++)
    {
        for( int ncol = box.y; ncol < box.y+box.height; ncol++)
        {
            float v = depth.at<float>(nrow, ncol);
            
            if(v<1000||v>6000)
		continue;
            
            n++;
            sum += v;
        }
    }
    cout<<"out depth loop"<<endl;
    if (n>0)
        return sum/n;
    else
	return 0.0;
}

int main(int argc, char * argv[])
{
    ros::init(argc, argv, "follower_TLD");
    ros::NodeHandle n;
    ros::Rate waiting_rate(30);
    
    ImageConverter ic;
    
    while(ic.ready == false)
    {
        ros::spinOnce();
        waiting_rate.sleep();
    } 
    Mat frame = ic.curr_image;
    if(vis==false)
    {	sub is;
	ros::ServiceServer service = n.advertiseService("givedata", &sub::chatterCallback,&is);
     	while(is.file == false)
    	{
        	ros::spinOnce();
        	waiting_rate.sleep();
    	}
    vis=true;
    box=is.box;}
    
    //ros::Publisher vel_pub = n.advertise<geometry_msgs::Twist>("cmd_vel_mux/input/teleop", 200);
    //ros::Publisher dep_pub = n.advertise<geometry_msgs::Twist>("/average_depth", 200);
    ros::Publisher givedata=n.advertise<xm_TLD::num>("tld_people_position_estimation",1000);
    xm_TLD::num data;
    //FileStorage fs;
    //fs.open("/home/cuiyufang/catkin_ws/src/follower/src/parameters.yml", FileStorage::READ);
    TLD tld;
    Mat last_gray;
    Mat first;
    FILE *bb_file;
    cout<<"Init OK"<<endl;
    if(ic.ready == true)
    {
        //Register mouse callback to draw the bounding box
        cvNamedWindow("TLD",CV_WINDOW_AUTOSIZE);
        cvSetMouseCallback( "TLD", mouseHandler, NULL );
        cout<<"cv set ok"<<endl;
        //read parameters file
        tld.read(1);
        ic.curr_image.copyTo(frame);
        cvtColor(frame, last_gray, CV_RGB2GRAY);
        frame.copyTo(first);
        cout<<"frame OK"<<endl;
        //Initialization
//GETBOUNDINGBOX:
        //while(!gotBB)
        //{
            first.copyTo(frame);
            cvtColor(frame, last_gray, CV_RGB2GRAY);
            drawBox(frame,box);
            imshow("TLD", frame);
            if (cvWaitKey(33) == 'q')
                return 0;
        //}
        /*if (min(box.width,box.height)<(int)fs.getFirstTopLevelNode()["min_win"]){
            cout << "Bounding box too small, try again." << endl;
            gotBB = false;
            goto GETBOUNDINGBOX;
        }*/
        cout<<"BB ok"<<endl;
        cvSetMouseCallback( "TLD", NULL, NULL );
        //Output file
        printf("Initial Bounding Box = x:%d y:%d h:%d w:%d\n",box.x,box.y,box.width,box.height);
        bb_file = fopen("bounding_boxes.txt","w");
        tld.init(last_gray,box,bb_file);
        cout<<"tld init ok"<<endl;
    }

    Mat current_gray;
    BoundingBox pbox;
    vector<Point2f> pts1;
    vector<Point2f> pts2;
    bool status=true;
    int frames = 1;
    int detections = 1;

    
    

    while(ros::ok())
    {
        //get frame
        ic.curr_image.copyTo(frame);
        cvtColor(frame, current_gray, CV_RGB2GRAY);
        //Process Frame
	
        tld.processFrame(last_gray,current_gray,pts1,pts2,pbox,status,tl,bb_file);
	if(tld.boxfile==false)
	{	
		while(tld.tldboxfile==false)
		{
			ros::spinOnce();
			waiting_rate.sleep();
		}
		if(tld.rightbox.width!=0)
      		{
			pbox.x=tld.rightbox.x+2*(tld.rightbox.width)/7;
			pbox.y=tld.rightbox.y+(tld.rightbox.height)/5;
			pbox.width=2*(tld.rightbox.width)/5;
			pbox.height=(tld.rightbox.height)/3;
			status=true;
			
			tld.box(pbox);
			
		}	
			
      }
        //Draw Points
        if (status)
        {
            //drawPoints(frame,pts1);
            //drawPoints(frame,pts2,Scalar(0,255,0));
            drawBox(frame,pbox);
            detections++;

           
            cout<<"computing ave_dep"<<endl;
            float ave_dep = average_depth(ic.curr_image_depth, pbox);
            cout<<"average depth = "<<ave_dep/1000.0<<endl;
	    /*fx = 525.0  # focal length x
	    fy = 525.0  # focal length y
	    cx = 319.5  # optical center x
	    cy = 239.5  # optical center y*/
	    float ux=(pbox.x+pbox.x+pbox.width)/2.0;
	    float uy=(pbox.y+pbox.y+pbox.height)/2.0;
	    float worldx=(ux-319.5)*(ave_dep)/525.0;
	    float worldy=(uy-239.5)*(ave_dep)/525.0;
	    data.a=1;
	    data.y=-worldx/1000.0;
	    data.z=-worldy/1000.0;
	    data.x=ave_dep/1000.0;
	    if(data.x>=0)
		{cout<<data.a<<" "<<data.x<<" "<<data.y<<" "<<data.z<<endl;
	    givedata.publish(data);}
           
            
        }
        //Display
        imshow("TLD", frame);
        waitKey(3);
        //swap points and images
        swap(last_gray,current_gray);
        pts1.clear();
        pts2.clear();
        frames++;
        ic.ready = false;

        while(ic.ready == false)
        {
            ros::spinOnce();
        }
    }
    
    fclose(bb_file); 
    return 0;
}
