# This contains the data structure of ANPP Packet 43. The Angular Acceleration Packet.
#
# Specified using the Advanced Navigation Packet Protocol
#
# ADV-153: Angular acceleration XYZ (rad/s/s), native FRD body frame. Published for diagnostics;
# the FRD->FLU static flip (x, -y, -z) would be applied the same way as angular_velocity/
# body_acceleration if a future consumer needs it in body/FLU frame.

# header.stamp specifies the ROS time this measurement was decoded.
#
# header.frame_id is the frame of reference of the device.
std_msgs/Header header

# ANPP Header
adnav_interfaces/ANPPHeader anpp_header

# Angular acceleration of the device in XYZ (rad/s/s), native FRD frame
geometry_msgs/Point angular_acceleration
