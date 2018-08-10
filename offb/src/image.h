#ifndef IMAGE_H_
#define IMAGE_H_

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <std_msgs/Float64.h>
#include <geometry_msgs/Point.h>
#include <ros/ros.h>
#include <iostream>

using namespace cv;
using namespace std;

void FindCircle();
void C_buffer_and_RFPS(unsigned int rfps);
void callback(const std_msgs::Float64 &msg);

bool show_frame = false;
bool flag;
int radius;
Mat recogClr, frame, dst;

ros::Publisher pub;
ros::Subscriber sub;
geometry_msgs::Point msg;

#endif
