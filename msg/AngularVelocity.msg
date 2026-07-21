# This contains the data structure of ANPP Packet 42. The Angular Velocity Packet.
#
# Specified using the Advanced Navigation Packet Protocol
#
# ADV-153: Angular velocity XYZ (rad/s), native FRD body frame - the lightweight replacement for
# Packet 20's embedded angular velocity field. Published raw here (FRD); the FRD->FLU static flip
# (x, -y, -z) is applied separately for imu.angular_velocity. Note: robot_localization's imu0
# consumes imu.angular_velocity, not this raw topic directly.

# header.stamp specifies the ROS time this measurement was decoded.
#
# header.frame_id is the frame of reference of the device.
std_msgs/Header header

# ANPP Header
adnav_interfaces/ANPPHeader anpp_header

# Angular velocity of the device in XYZ (rad/s), native FRD frame
geometry_msgs/Point angular_velocity
