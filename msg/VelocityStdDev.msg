# This contains the data structure of ANPP Packet 25. The Velocity Standard Deviation Packet.
#
# Specified using the Advanced Navigation Packet Protocol
#
# ADV-153: standard deviation of North/East/Down velocity (m/s) - intentionally kept in the same
# NED nav frame as Packet 35 (NedVelocity). See the design doc's "single-rotation" design: the
# bridge package rotates this covariance and Packet 35's mean into body frame together, in one
# operation, using the same current-orientation rotation matrix, so the mean and covariance can
# never end up inconsistent with each other.

# header.stamp specifies the ROS time this measurement was decoded.
#
# header.frame_id is the frame of reference of the device.
std_msgs/Header header

# ANPP Header
adnav_interfaces/ANPPHeader anpp_header

# North, East, Down velocity standard deviation (m/s)
adnav_interfaces/NED standard_deviation
