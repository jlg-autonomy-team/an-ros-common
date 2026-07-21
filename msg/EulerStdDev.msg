# This contains the data structure of ANPP Packet 26. The Euler Orientation Standard Deviation
# Packet.
#
# Specified using the Advanced Navigation Packet Protocol
#
# ADV-153: standard deviation of Roll/Pitch/Heading (rad). Published for diagnostics and also
# wired directly into imu.orientation_covariance (roll sigma -> [3][3], pitch sigma -> [4][4],
# heading sigma -> [5][5]) - the NED->ENU yaw sign flip applied to the mean heading does not
# change its variance, so no extra conversion is needed here.

# header.stamp specifies the ROS time this measurement was decoded.
#
# header.frame_id is the frame of reference of the device.
std_msgs/Header header

# ANPP Header
adnav_interfaces/ANPPHeader anpp_header

# Roll, Pitch, Heading standard deviation (rad)
adnav_interfaces/RPH standard_deviation
