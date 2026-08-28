# This contains the data structure of ANPP Packet 25. The Velocity Standard Deviation Packet.
#
# Specified using the Advanced Navigation Packet Protocol
#
# ADV-153: standard deviation of North/East/Down velocity (m/s), retained in the native NED
# navigation frame. The current bridge uses these values as the diagonal covariance for the
# packet-20 linear velocity copied into /ins/twist; it does not rotate them or consume Packet 35.

# header.stamp specifies the ROS time this measurement was decoded.
#
# header.frame_id is the frame of reference of the device.
std_msgs/Header header

# ANPP Header
adnav_interfaces/ANPPHeader anpp_header

# North, East, Down velocity standard deviation (m/s)
adnav_interfaces/NED standard_deviation
