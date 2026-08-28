# This contains the data structure of ANPP Packet 24. The Position Standard Deviation Packet.
#
# Specified using the Advanced Navigation Packet Protocol
#
# ADV-153: standard deviation of latitude/longitude/height, already expressed in metres (not
# degrees/radians) by the device - a local-level (North/East/Up-magnitude) decomposition, not a
# raw ECEF standard deviation. See docs/02-data-and-frame-contracts.md in certus_eval for the
# current contract and source provenance. The bridge maps these values onto local ENU position
# covariance without needing a rotation.

# header.stamp specifies the ROS time this measurement was decoded.
#
# header.frame_id is the frame of reference of the device.
std_msgs/Header header

# ANPP Header
adnav_interfaces/ANPPHeader anpp_header

# Latitude, Longitude, Height standard deviation (m)
adnav_interfaces/LLH standard_deviation
