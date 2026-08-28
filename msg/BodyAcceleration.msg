# This contains the data structure of ANPP Packet 38. The Body Acceleration Packet.
#
# Specified using the Advanced Navigation Packet Protocol
#
# ADV-153: raw Packet 38 body acceleration. This topic is diagnostic only in the current
# pipeline; /ins/imu uses Packet 28 raw sensor acceleration instead.

# header.stamp specifies the ROS time this measurement was decoded.
#
# header.frame_id is the frame of reference of the device.
std_msgs/Header header

# ANPP Header
adnav_interfaces/ANPPHeader anpp_header

# Body acceleration in XYZ (m/s/s), native FRD frame
geometry_msgs/Point body_acceleration

# g_force
float32 g_force
