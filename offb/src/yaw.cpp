#include <ros/ros.h>
#include <mavros_msgs/OverrideRCIn.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/CommandBool.h>
#include <tf/transform_datatypes.h>
#include <std_msgs/Float64.h>
#include <mavros_msgs/State.h>
#include <std_msgs/Bool.h>
#include <mavros_msgs/CommandTOL.h>
//#include <std_msgs/Int16.h>

ros::Publisher yaw_pub;
ros::Subscriber ComPose_sub;
std_msgs::Float64 yaw_msg;
//ros::Subscriber sub;
double quatx, quaty, quatz, quatw;
double roll, pitch, yaw;
//int val_data;
void ComPoseCallback(const geometry_msgs::PoseStamped& msg){
	
  quatx = msg.pose.orientation.x;
	quaty = msg.pose.orientation.y;
	quatz = msg.pose.orientation.z;
	quatw = msg.pose.orientation.w;
	tf::Quaternion q(quatx, quaty, quatz, quatw);
	tf::Matrix3x3 m(q);
	m.getRPY(roll, pitch, yaw);
  yaw_msg.data = yaw;
	//yaw_pub.publish(yaw_msg);
	return ;

}

//void valCallback(const std_msgs::Int16& msg){
  //val_data = msg.data;
//}


int main(int argc, char **argv)
{	
	ros::init(argc, argv, "Arduino_Breathe");
	ros::NodeHandle nh;
  ComPose_sub = nh.subscribe("/mavros/local_position/pose", 10, ComPoseCallback);
	yaw_pub= nh.advertise<std_msgs::Float64>("/Matlab/yaw",10);
  //sub = nh.subscribe("chatter",10,valCallback);
  ros::Rate rate(10.0);
  
  while(ros::ok()){
      //ComPose_sub = nh.subscribe("/mavros/local_position/pose", 100, ComPoseCallback);
      ROS_INFO("Roll: [%f],Pitch: [%f],Yaw: [%f]",roll,pitch,yaw);
      //ROS_INFO("Value:%i",val_data);
      yaw_pub.publish(yaw_msg);
      ros::spinOnce();
      rate.sleep();
  }
  
}
 
 
 
 