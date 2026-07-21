# This contains the data structure of ANPP Packet 38. The Body Acceleration Packet.
#
# Specified using the Advanced Navigation Packet Protocol
#
# ADV-153: Body acceleration XYZ (m/s/s), native FRD body frame - the lightweight replacement for
# Packet 20's embedded body acceleration field. Published raw here (FRD); the FRD->FLU static
# flip (x, -y, -z) is applied separately for imu.linear_acceleration.

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
