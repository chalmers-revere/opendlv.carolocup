# OpenDLV - Software for CaroloCup realized with OpenDLV and OpenDaVINCI
# Copyright (C) 2016 Chalmers
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

project(carolocup-control)

add_subdirectory(laneFollower)
add_subdirectory(overtaker)
#add_subdirectory(parker)
add_subdirectory(proxy/communicationLink)
add_subdirectory(proxy/cameraProxy)
add_subdirectory(proxy/serialHandler)
add_subdirectory(proxy/udpconnectionreceiver)
add_subdirectory(proxy/udpconnectionstreamer)
#add_subdirectory(proxy/lidarHandler)  ### FIRST BUILD COMPLETE THEN UNCOMMNET THIS AND BUILD INCREMENTAL
