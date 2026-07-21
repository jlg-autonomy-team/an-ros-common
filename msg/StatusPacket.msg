# This contains the data structure of ANPP Packet 23. The Status Packet.
#
# Specified using the Advanced Navigation Packet Protocol
#
# ADV-153: lightweight replacement for reading System Status / Filter Status off the
# monolithic Packet 20 (System State Packet) - same two bitfields, without the
# position/velocity/orientation payload we don't need for status/health monitoring.

# header.stamp specifies the ROS time this measurement was decoded.
#
# header.frame_id is the frame of reference of the device.
std_msgs/Header header

# ANPP Header
adnav_interfaces/ANPPHeader anpp_header

# System Status 16 bit boolean array
adnav_interfaces/SystemStatus system_status

# Filter Status 16 bit boolean array
adnav_interfaces/FilterStatus filter_status
