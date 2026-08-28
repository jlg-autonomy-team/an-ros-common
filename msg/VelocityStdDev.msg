# This contains the data structure of ANPP Packet 25. The Velocity Standard Deviation Packet.
#
# Specified using the Advanced Navigation Packet Protocol
#
# ADV-153: standard deviation of North/East/Down velocity (m/s), retained in the native NED
# navigation frame. The current bridge uses these values as the diagonal covariance for packet-20
# velocity and rotates both together into the imu_link body frame for /ins/twist.

# header.stamp specifies the ROS time this measurement was decoded.
#
# header.frame_id is the frame of reference of the device.
std_msgs/Header header

# ANPP Header
adnav_interfaces/ANPPHeader anpp_header

# North, East, Down velocity standard deviation (m/s)
adnav_interfaces/NED standard_deviation
