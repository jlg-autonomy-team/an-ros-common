# This contains the data structure of ANPP Packet 51. The Odometer State Packet.
#
# Specified using the Advanced Navigation Packet Protocol

# header.stamp specifies the ROS time this measurement was decoded.
#
# header.frame_id is the frame of reference of the device.
std_msgs/Header header

# ANPP Header
adnav_interfaces/ANPPHeader anpp_header

# Accumulated odometer pulse count
int32 pulse_count

# Distance travelled (m)
float64 distance

# Speed (m/s)
float64 speed

# Slip estimate
float64 slip

# True when the odometer input is currently in use by the filter
bool active
