# This contains the data structure of ANPP Packet 36. The Body Velocity Packet.
#
# Specified using the Advanced Navigation Packet Protocol
#
# ADV-153: North/East/Down velocity (m/s), native NED nav frame - the lightweight replacement for
# Packet 20's embedded NED velocity field. Deliberately NOT rotated into body frame at this raw
# message level; that rotation happens once in the bridge package, applied together with Packet
# 25's covariance using the same current-orientation rotation matrix (see the design doc's
# "single-rotation" design under S3).

# header.stamp specifies the ROS time this measurement was decoded.
#
# header.frame_id is the frame of reference of the device.
std_msgs/Header header

# ANPP Header
adnav_interfaces/ANPPHeader anpp_header

# Velocity in the body frame (m/s). This packet contains no uncertainty estimate.
geometry_msgs/Vector3 velocity
