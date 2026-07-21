# This contains the data structure of ANPP Packet 40. The Quaternion Orientation Packet.
#
# Specified using the Advanced Navigation Packet Protocol
#
# ADV-153: quaternion orientation as delivered by the device (NED body->nav rotation). The device
# transmits QS/QX/QY/QZ as a SCALAR-FIRST quaternion; when the decoder populates this
# geometry_msgs/Quaternion (scalar-LAST: x,y,z,w), map QS->w, QX->x, QY->y, QZ->z.
#
# NOT YET FRAME-CORRECTED: unlike EulerOrientation/imu.orientation, the equivalent of the
# yaw_enu = pi/2 - heading remap has not been derived/validated for the quaternion form (see the
# design doc S8 risk list). Published for diagnostics/future validation only - not consumed by
# the locked-in Option A + robot_localization pipeline.

# header.stamp specifies the ROS time this measurement was decoded.
#
# header.frame_id is the frame of reference of the device.
std_msgs/Header header

# ANPP Header
adnav_interfaces/ANPPHeader anpp_header

# Orientation of the device as a quaternion, native NED body->nav convention, uncorrected
geometry_msgs/Quaternion orientation
