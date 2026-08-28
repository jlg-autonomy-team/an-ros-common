# This contains the data structure of ANPP Packet 36. The Body Velocity Packet.
#
# Specified using the Advanced Navigation Packet Protocol
#
# ADV-153: raw Packet 36 velocity. This topic is diagnostic only in the current pipeline; the
# bridge does not consume or synthesize body velocity for /ins/twist.

# header.stamp specifies the ROS time this measurement was decoded.
#
# header.frame_id is the frame of reference of the device.
std_msgs/Header header

# ANPP Header
adnav_interfaces/ANPPHeader anpp_header

# Velocity fields supplied by the device (m/s). This packet contains no uncertainty estimate.
geometry_msgs/Vector3 velocity
