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
bool checkflag = false, arm_flag, alt_flag = true;
double Roll, Pitch, Throttle, Yaw;
double quatx, quaty, quatz, quatw;

mavros_msgs::OverrideRCIn Rc_msg;

ros::Subscriber sub;
ros::Subscriber state_sub;
ros::Subscriber ComPose_sub;
ros::Subscriber checkflag_sub;
ros::Publisher pub;
ros::Publisher yaw_pub;
ros::Publisher local_pos_pub;
ros::ServiceClient arming_client;
ros::ServiceClient set_mode_client;

mavros_msgs::State current_state;

void checkflag_cb(const std_msgs::Bool& msg){
  checkflag = msg.data;
}

void state_cb(const mavros_msgs::State::ConstPtr& msg){
  current_state = *msg;
}

void myCallback(const geometry_msgs::Twist& msg){
  //mavros_msgs::OverrideRCIn Rc_msg;
	Roll = msg.angular.x;
	Pitch = msg.angular.y;
	Yaw = msg.angular.z;
	Throttle = msg.linear.z;
 
	Rc_msg.channels[0] = Roll;
	Rc_msg.channels[1] = Pitch;
	Rc_msg.channels[2] = Throttle;  //Throttle;
	Rc_msg.channels[3] = Yaw;  //Yaw;
	Rc_msg.channels[4] = 0;
	Rc_msg.channels[5] = 0;
	Rc_msg.channels[6] = 0;
	Rc_msg.channels[7] = 0;
	
	pub.publish(Rc_msg);
	
}

void ComPoseCallback(const geometry_msgs::PoseStamped& msg){
	std_msgs::Float64 yaw_msg;
  quatx = msg.pose.orientation.x;
	quaty = msg.pose.orientation.y;
	quatz = msg.pose.orientation.z;
	quatw = msg.pose.orientation.w;
	tf::Quaternion q(quatx, quaty, quatz, quatw);
	tf::Matrix3x3 m(q);
	double roll, pitch, yaw;
	m.getRPY(roll, pitch, yaw);
	//ROS_INFO("Roll: [%f],Pitch: [%f],Yaw: [%f]",roll,pitch,yaw);
	
  yaw_msg.data = yaw;
	yaw_pub.publish(yaw_msg);
	//return ;

}

//ros::AsyncSpinner spinner(4);

int main(int argc, char **argv)
{	
  ros::init(argc, argv, "GoMission");
  ros::NodeHandle nh;
  
  state_sub = nh.subscribe<mavros_msgs::State>("/mavros/state", 10, state_cb);
  checkflag_sub = nh.subscribe("/checkflag", 10, checkflag_cb);
  sub = nh.subscribe("/custom/rc/input", 1000, myCallback);
  ComPose_sub = nh.subscribe("/mavros/local_position/pose", 100, ComPoseCallback);
 
  pub = nh.advertise<mavros_msgs::OverrideRCIn>("/mavros/rc/override", 10); 
  local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>("/mavros/setpoint_position/local", 10);
  yaw_pub= nh.advertise<std_msgs::Float64>("/Matlab/yaw",10);
  arming_client = nh.serviceClient<mavros_msgs::CommandBool>("mavros/cmd/arming");
  set_mode_client = nh.serviceClient<mavros_msgs::SetMode>("mavros/set_mode");
  
  ros::Rate rate(100.0);
  while(ros::ok() && !current_state.connected){
    ros::spinOnce();
    rate.sleep();
  } 
 
  

 
  /*std_msgs::Bool switch_mode_msg; 
 
	geometry_msgs::PoseStamped pose;
	pose.pose.position.x=0;
	pose.pose.position.y=0;
	pose.pose.position.z=1;
	for (int i=50;ros::ok() && i>0;--i){
		local_pos_pub.publish(pose);
		ros::spinOnce();
		rate.sleep();
	}*/
  
  mavros_msgs::SetMode offb_set_mode;
  offb_set_mode.request.custom_mode = "MANUAL";
  
  /*mavros_msgs::CommandBool arm_cmd;
  arm_cmd.request.value = true;*/
  
  ros::Time last_time = ros::Time::now();
	
	while(ros::ok()){
		
    //checkflag_sub = nh.subscribe("/checkflag", 10, checkflag_cb);
    //sub = nh.subscribe("/custom/rc/input", 1000, myCallback);
		//ComPose_sub = nh.subscribe("/mavros/local_position/pose", 100, ComPoseCallback);
		
    if (current_state.mode != "MANUAL" && ((ros::Time::now() - last_time) > ros::Duration(5.0)) && !checkflag){
      if(set_mode_client.call(offb_set_mode) && offb_set_mode.response.mode_sent){
        ROS_INFO("MANUAL enabled");
        //arm_flag = 1;
      }
      
      last_time = ros::Time::now();
    }
    /*else if (arm_flag == 1){
      if( !current_state.armed && (ros::Time::now() - last_time > ros::Duration(5.0)) && !checkflag){
        if (arming_client.call(arm_cmd) && arm_cmd.response.success){
          ROS_INFO("Vehicle armed");
        }
        arm_flag = 0;
        last_time = ros::Time::now();
      } 
    }*/
    
    else if (current_state.mode != "ALTCTL" && ((ros::Time::now() - last_time) > ros::Duration(5.0)) && checkflag == 1){
      //ros::ServiceClient land_cl = nh.serviceClient<mavros_msgs::CommandTOL>("/mavros/cmd/land");
      //mavros_msgs::CommandTOL srv_land;
      /*if (alt_flag == 1){
        offb_set_mode.request.custom_mode = "ALTCTL";
      }*/
        if(set_mode_client.call(offb_set_mode) && offb_set_mode.response.mode_sent){
          ROS_INFO("ALTCTL enabled");
        }
        //arm_flag = 1;
        //alt_flag = 0;
        last_time = ros::Time::now();
      }
      /*if (current_state.mode == "AUTO.LAND"){
        if(land_cl.call(srv_land)){
          ROS_INFO("srv_land send ok %d", srv_land.response.success);
        }
      }*/
    
    

    pub.publish(Rc_msg);
		//local_pos_pub.publish(pose);
    //spinner.start();
    //ros::waitForShutdown();
		ros::spinOnce();
		rate.sleep();
	}
	
}


/*if (current_state.mode != "ALTCTL" && ((ros::Time::now() - last_time) > ros::Duration(5.0)) && !checkflag){
      if(set_mode_client.call(offb_set_mode) && offb_set_mode.response.mode_sent){
        ROS_INFO("ALTCTL enabled");
      }
      last_time = ros::Time::now();
    }
    else {
      if( !current_state.armed && (ros::Time::now() - last_time > ros::Duration(5.0)) && !checkflag){
        if (arming_client.call(arm_cmd) && arm_cmd.response.success){
          ROS_INFO("Vehicle armed");
        }
        last_time = ros::Time::now();
      } 
    }
    
    if (checkflag == 1){
      //ros::ServiceClient land_cl = nh.serviceClient<mavros_msgs::CommandTOL>("/mavros/cmd/land");
      //mavros_msgs::CommandTOL srv_land;
      offb_set_mode.request.custom_mode = "STABILIZED";
      if (current_state.mode != "STABILIZED" && ((ros::Time::now() - last_time) > ros::Duration(5.0))){
        if(set_mode_client.call(offb_set_mode) && offb_set_mode.response.mode_sent){
          ROS_INFO("STABILIZED enabled");
        }
        last_time = ros::Time::now();
      }
      /*if (current_state.mode == "AUTO.LAND"){
        if(land_cl.call(srv_land)){
          ROS_INFO("srv_land send ok %d", srv_land.response.success);
        }
      }*/


/*if(land_cl.call(srv_land)){
        ROS_INFO("srv_land send ok %d", srv_land.response.success);
    }else{
        ROS_ERROR("Failed Land");
    }
*/
