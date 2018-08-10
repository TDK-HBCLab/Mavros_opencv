//節點名稱"circle_tracking_node"
//發布訊息"center_position"


#include <ros/ros.h>
#include <geometry_msgs/Point.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;

ros::Publisher pub;
geometry_msgs::Point msg;
std::deque<Point2f> trac(32);

int main(int argc, char **argv)
{
	
	VideoCapture capture(0);
  capture.set(CV_CAP_PROP_FRAME_WIDTH,320);
  capture.set(CV_CAP_PROP_FRAME_HEIGHT,240);
	ros::init(argc, argv, "circle_node");
	ros::NodeHandle nh;
	
  pub = nh.advertise<geometry_msgs::Point>("center_position",1000);
	while (/*(char(waitKey(1)) != 27) ||*/ (ros::ok())) {

		Mat frame;
		capture >> frame;
		Mat midImage;
		//resize(frame, frame, Size(240, 320), 0, 0, CV_INTER_AREA);
		//cvtColor(frame, midImage, COLOR_BGR2HSV);
    //flip(frame,frame,1);
    cvtColor(frame, midImage, COLOR_BGR2GRAY);
		GaussianBlur(midImage, midImage, Size(9, 9), 2, 2);
    threshold(midImage,midImage,50,255,THRESH_BINARY_INV);
		//inRange(midImage, Scalar(0, 0, 0, 0), Scalar(360, 255, 30, 0), midImage);
		Mat element = getStructuringElement(MORPH_RECT, Size(10, 10)); //15,15
		dilate(midImage, midImage, element);
		erode(midImage, midImage, element);
    imshow("erode",midImage);
		Mat not_img;
		bitwise_not(midImage, not_img);
    imshow("Not Image", not_img);
		Mat edge;
		Canny(not_img, edge, 20, 160, 3);  //trackbar_test

		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
    
		RNG rng(12345);
		findContours(edge, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); 
		vector<Point2f> center(contours.size());
		vector<float> radius(contours.size());

		for (int i = 0; i<contours.size(); i++) {
			minEnclosingCircle(contours[i], center[i], radius[i]);
			if (radius[i] > 40) {
				circle(frame, center[i], radius[i], Scalar(255, 0, 0), 3, CV_AA, 0);
				circle(frame, center[i], 2, Scalar(0, 0, 255), 3, CV_AA, 0);
				msg.x = (int) center[i].x;
				msg.y = (int) center[i].y;
				msg.z = 1;
				cout << "x= " << center[i].x << ", y= " << center[i].y << endl;
				ROS_INFO_STREAM("Found a ball at ("<<msg.x<<", "<<msg.y <<")");
				
			} //if
			center[0].x = 0;
			center[0].y = 0;
			msg.z = 0;
		} //for
    	std::deque<Point2f>::iterator it = trac.begin();
		int cnt=0;
		while(it != trac.end()){
			int thickness = 4 - cnt++/8;
      Point2f p = *it;
      Point2f p2 = *(it+1);
			if(p.x==0 || p2.x==0) {
				it++;
				continue;
			}
			else if(thickness>0) line(frame, *it, *(it+1), Scalar(0, 255, 0), thickness );
			else{
				trac.pop_back();
				continue;
			}
			it++;
		}
    if (trac.size()>=32) trac.pop_back();
    ROS_DEBUG_STREAM(trac.size());
		if (msg.z == 0) ROS_ERROR_STREAM("Where's the ball???");
		pub.publish(msg);
		line(frame, Point(320,220), Point(320,260), Scalar(0, 0, 0), 2, 8);
		line(frame, Point(300,240), Point(340,240), Scalar(0, 0, 0), 2, 8);
    imshow("From Webcam", frame);
    waitKey(1);
    //imshow("erode",midImage);
		//imshow("contours", frame);


	} //while
	return 0;
}

