# Example - Example code.
# Copyright (C) 2016 Christian Berger
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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(carolocup-control-serial)

###########################################################################
# Set the search path for .cmake files.
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake.Modules" ${CMAKE_MODULE_PATH})

# Add a local CMake module search path dependent on the desired installation destination.
# Thus, artifacts from the complete source build can be given precendence over any installed versions.
IF (UNIX)
    SET(CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/share/cmake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}/Modules" ${CMAKE_MODULE_PATH})
ENDIF ()
IF (WIN32)
    SET(CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/CMake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}/Modules" ${CMAKE_MODULE_PATH})
ENDIF ()

###########################################################################
# Include flags for compiling.
INCLUDE(CompileFlags)

###########################################################################
# Find and configure CxxTest.
INCLUDE(CheckCxxTestEnvironment)

###########################################################################
# Find OpenDaVINCI.
FIND_PACKAGE(OpenDaVINCI REQUIRED)

###########################################################################
# Find ODVDCaroloCupDataModel.
find_package(ODVDCaroloCupDataModel REQUIRED)

###########################################################################
# Find GLib
FIND_PACKAGE(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)

LINK_DIRECTORIES(${GLIB_LIBRARY_DIRS})
ADD_DEFINITIONS(${GLIB_CFLAGS_OTHER})

#target_link_libraries (gtk-test ${GLIB_LIBRARIES})

###############################################################################
# Set header files from ODVDCaroloCupDataModel.
INCLUDE_DIRECTORIES(SYSTEM ${ODVDCAROLOCUPDATAMODEL_INCLUDE_DIRS})
# Set header files from GLib.
INCLUDE_DIRECTORIES(SYSTEM ${GLIB_INCLUDE_DIRS})
# Set header files from OpenDaVINCI.
INCLUDE_DIRECTORIES(SYSTEM ${OPENDAVINCI_INCLUDE_DIRS})
# Set include directory.
INCLUDE_DIRECTORIES(include)

# Set libraries to link against.
set(LIBRARIES ${OPENDAVINCI_LIBRARIES}
        ${ODVDCAROLOCUPDATAMODEL_LIBRARIES}
        ${GLIB_LIBRARIES})


