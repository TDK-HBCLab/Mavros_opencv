#!/usr/bin/env python
import rospy
import RPi.GPIO as GPIO
import time
from std_msgs.msg import Float32 
#import sys
trigger_pin = 23
echo_pin = 24


GPIO.setmode(GPIO.BCM)
GPIO.setup(trigger_pin, GPIO.OUT)
GPIO.setup(echo_pin, GPIO.IN)

def send_trigger_pulse():
    GPIO.output(trigger_pin, True)
   
    time.sleep(0.000000001)  ##this is the best 0.0000000001,25  0.00000000002,35
    GPIO.output(trigger_pin, False)

def wait_for_echo(value, timeout):
    count = timeout
    while GPIO.input(echo_pin) != value and count > 0:
        count = count - 1
    if count == 0:
		      rospy.logwarn("timeout")
	

def main():
    pub = rospy.Publisher('ultrasonic_data', Float32, queue_size=10)
    rospy.init_node('ultrasonic' , anonymous = True)
    rate = rospy.Rate(15)  ##35
    while not rospy.is_shutdown():
        send_trigger_pulse()
        wait_for_echo(True, 5000)
        start = time.time() ##rospy.Time.now()
        wait_for_echo(False, 5000)
        finish = time.time()##rospy.Time.now()
        pulse_len = finish - start
        distance_cm = pulse_len * 340 * 100 * 0.5
        #rospy.loginfo(distance_cm)
        #show_distance = "cm = %f" % dsitance_cm
        #rospy.loginfo(show_distance)
        pub.publish(distance_cm)
        rate.sleep()
        #return distance_cm
if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:##rospy.ROSInterruptException:
        GPIO.cleanup()
    
#while True:
    #print("cm=%f\n" % main())
    #time.sleep(1)
