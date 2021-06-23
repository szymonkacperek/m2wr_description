#!/usr/bin/env python3

import rospy
from geometry_msgs.msg import Twist

def main():
    rospy.init_node('basic_control')
    pub = rospy.Publisher('/cmd_vel', Twist, queue_size=1)
    rate = rospy.Rate(20)
    twist_msg = Twist()
    twist_msg.linear.x = 0.3
    while not rospy.is_shutdown():
        pub.publish(twist_msg)
        rate.sleep()

if __name__=='__main__':
    try:
        main()
    except rospy.ROSInterruptException:
        pass
