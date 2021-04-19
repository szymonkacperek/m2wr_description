#!/usr/bin/env python3
import rospy
import sys
import tf
import tf2_ros
import geometry_msgs.msg

if __name__=='__main__':
    if len(sys.argv) < 8:
        rospy.logerr('Invalid number of parameters')
        sys.exit(0)

    else:
        if sys.argv[1] == 'world':
            rospy.logerr('Your static turle name cannot be "world"')
            sys.exit(0)

        rospy.init_node('my_static_tf2_broadcaster')
        broadcaster = tf2_ros.StaticTransformBroadcaster()
        static_transformStamped = geometry_msgs.msg.TransformStamped()
        
        static_transformStamped.header.stamp = rospy.Time.now()
        static_transformStamped.header.frame_id = "world"
        static_transformStamped.child_frame_id = sys.argv[1]

        static_transformStamped.transform.translation.x = float(sys.argv[2])
        static_transformStamped.transform.translation.y = float(sys.argv[3])
        static_transformStamped.transform.translation.x = float(sys.argv[4])

        quat = tf.transformations.quaternion_from_euler(float(sys.argv[5]), float(sys.argv[6]), float(sys.argv[7]))
        static_transformStamped.transform.rotation.x = quat[0]
        static_transformStamped.transform.rotation.x = quat[1]
        static_transformStamped.transform.rotation.x = quat[2]
        static_transformStamped.transform.rotation.x = quat[3]

        broadcaster.sendTransform(static_transformStamped)
        rospy.spin()
        

