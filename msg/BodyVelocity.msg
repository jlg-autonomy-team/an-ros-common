# This contains the data structure of ANPP Packet 36. The Body Velocity Packet.
#
# Specified using the Advanced Navigation Packet Protocol
#
# ADV-153: raw Packet 36 velocity. This topic is diagnostic only in the current pipeline; the
# bridge intentionally uses matched Packet 20 plus Packet 25 and rotates both with one matrix for
# /ins/twist; this Packet 36 topic remains diagnostic-only.

# header.stamp specifies the ROS time this measurement was decoded.
#
# header.frame_id is the frame of reference of the device.
std_msgs/Header header

# ANPP Header
adnav_interfaces/ANPPHeader anpp_header

# Velocity fields supplied by the device (m/s). This packet contains no uncertainty estimate.
geometry_msgs/Vector3 velocity
