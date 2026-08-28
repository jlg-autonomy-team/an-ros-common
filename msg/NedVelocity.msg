# This contains the data structure of ANPP Packet 35. The NED Velocity Packet.
#
# Specified using the Advanced Navigation Packet Protocol
#
# ADV-153: North/East/Down velocity (m/s), native NED navigation frame. This is a raw diagnostic
# topic only; the current bridge uses packet-20 velocity plus packet-25 covariance for /ins/twist
# and rotates the matched pair into the imu_link body frame.

# header.stamp specifies the ROS time this measurement was decoded.
#
# header.frame_id is the frame of reference of the device.
std_msgs/Header header

# ANPP Header
adnav_interfaces/ANPPHeader anpp_header

# Velocity in North, East, Down (m/s)
adnav_interfaces/NED velocity
