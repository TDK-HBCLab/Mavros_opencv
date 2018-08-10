#include "image.h"
#include <geometry_msgs/Twist.h>
unsigned int my_FPS = 60;
VideoCapture cap(0);
ros::Subscriber l_sub;
int efrt_roll, efrt_pitch;
bool f_line = 0;
void draw_line(const geometry_msgs::Twist &msg);
int main(int argc, char**argv){
  cap.set(CV_CAP_PROP_FRAME_WIDTH,320);  //cap.set(3,320);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT,240); //cap.set(4,240);
  cap.set(CAP_PROP_FPS,my_FPS);
  ros::init(argc, argv, "Image");
  ros::NodeHandle nh;
  pub = nh.advertise<geometry_msgs::Point>("circle_center_data",10);
  sub = nh.subscribe("stepflag",10, callback); 
  l_sub = nh.subscribe("custom/rc/input", 10, draw_line);
  /*if(argc>1){
    if(argv[1][0]=='y' || argv[1][0]=='Y'){
      show_frame= true;
    }
  }*/
  ros::Rate rate(60);
  while (cap.isOpened() && ros::ok()){
        cap >> frame;
        C_buffer_and_RFPS(40);  //clean buffer 10 frame. cap.grab();     
        FindCircle();
	pub.publish(msg);
	
        ros::spinOnce();
        rate.sleep();
	imshow("Video",frame);
	waitKey(1);
  }
}
//-----------------------------------------------------------------------------------




//--------------------------------------Subroutines----------------------------------

void FindCircle(){
  vector<Vec3f> circles;
  cvtColor(frame, dst, COLOR_BGR2GRAY);
  GaussianBlur(dst, dst, Size(5,5), 1.3, 1.3);
  HoughCircles(dst, circles, CV_HOUGH_GRADIENT, 2, 50, 200, 100, 0, 320); //30
  if(circles.size() > 0) f_line = 1;
  for (int i=0; i<circles.size(); i++){
    Point center(circles[i][0],circles[i][1]);   //declare variable that are two integer type variables(x,y).
    Point g_center(circles[i][0],circles[i][1]);
    if (f_line == true){
      line(frame, Point(g_center), Point(( g_center.x + efrt_roll), ( g_center.y  + efrt_pitch)), Scalar(0, 0, 255), 5, CV_AA);
      //line(frame, g_center, (g_center[0][0], (g_center[0][1] + efrt_pitch)), Scalar(0, 0, 255), 5, CV_AA);
      f_line = 0;
    }
    radius=cvRound(circles[i][2]);  //rounding the data number before assign to radius.
    circle(frame, center, radius, Scalar(0,0,255), 1, 8, 0);  //drawing a circle and the circle is solid
    circle(frame, center, 1, Scalar(0,255,0), 1, 8, 0); 
    
    ROS_INFO("Center X: [%i] , Y: [%i] ",center.x,center.y);
    msg.x = center.x;      
    msg.y = center.y;
    msg.z = radius;
    pub.publish(msg);
    
  }   //for
  
}

void C_buffer_and_RFPS(unsigned int rfps){
  static unsigned int count = 0;
  rfps = my_FPS / (my_FPS - rfps);
  count++; 
  if (rfps >= count){ 
    cap.grab();
    count = 0;
  }
}

void callback(const std_msgs::Float64 &msg){
  flag = msg.data;
}

void draw_line(const geometry_msgs::Twist &msg){
  efrt_roll = (int) msg.angular.x;
  efrt_pitch = (int) msg.angular.y;
  efrt_roll = (efrt_roll - 1500) * 2;
  efrt_pitch = (efrt_pitch - 1500) * 2;
}
