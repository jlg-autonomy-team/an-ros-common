# This contains the data structure of ANPP Packet 39. The Euler Orientation Packet.
#
# Specified using the Advanced Navigation Packet Protocol
#
# ADV-153: Roll, Pitch, Heading (rad) - the lightweight replacement for Packet 20's embedded
# orientation field. Published raw here (roll/pitch unchanged, heading is true-north-referenced,
# NED body->nav convention); the yaw_enu = pi/2 - heading remap is applied separately for
# imu.orientation.

# header.stamp specifies the ROS time this measurement was decoded.
#
# header.frame_id is the frame of reference of the device.
std_msgs/Header header

# ANPP Header
adnav_interfaces/ANPPHeader anpp_header

# Orientation of the device in Roll, Pitch, Heading (rad)
adnav_interfaces/RPH orientation
