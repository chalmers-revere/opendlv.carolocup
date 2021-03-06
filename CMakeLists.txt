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

cmake_minimum_required(VERSION 2.8)

# To create a proper build environment, you are supposed to call CMake as follows:
#
# PATH=/opt/od4/bin:$PATH cmake -D CMAKE_INSTALL_PREFIX=<Where shall Chalmers.Revere.scaledcars be installed?> 
#                               -D OPENDAVINCI_DIR=<Where is OpenDaVINCI installed?> 
#                               -D CXXTEST_INCLUDE_DIR=<Where is CxxTest installed?> ..
#
# The adjusted PATH variable is required to find odDataStructureGenerator-latest.jar.
# CMAKE_INSTALL_PREFIX specifies where to install Chalmers.Revere.scaledcars.
# OPENDAVINCI_DIR specifies where OpenDaVINCI is installed (if different from /usr/include and /usr/lib).
# CXXTEST_INCLUDE_DIR specifies where the UnitTest environment CxxTest can be found.

project(carolocup.software)

###########################################################################
# If dpkg and rpmbuild are available, .deb and .rpm packages will be
# created. Otherwise, the software is simply installed.
IF(     NOT("${DPKG_EXECUTABLE}" STREQUAL "DPKG_EXECUTABLE-NOTFOUND")
    AND NOT("${RPMBUILD_EXECUTABLE}" STREQUAL "RPMBUILD_EXECUTABLE-NOTFOUND")
    AND ("${PACKAGING_ENABLED}" STREQUAL "YES") )
    MESSAGE(STATUS "Enabling building .deb and .rpm packages.")
    SET(MAKE_PACKAGE package)
ELSE()
    # If the platform does not provide dpkg and rpmbuild, make package is
    # replaced with make help to simply have a no-op.
    SET(MAKE_PACKAGE help)
ENDIF()

###########################################################################
# Set the search path for .cmake files.
SET (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake.Modules" ${CMAKE_MODULE_PATH})
SET (CMAKE_MODULE_PATH "${OPENDAVINCI_DIR}/share/cmake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}/Modules" ${CMAKE_MODULE_PATH})

###########################################################################
# Enable the configuration of external projects, as used below.
include(ExternalProject)

###########################################################################
# Java is needed to run the odDataStructureGenerator tool
# for generating the data structures.
include(FindJava REQUIRED)

###########################################################################
# Find odDataStructureGenerator.
include(FindodDataStructureGenerator REQUIRED)

###########################################################################
# Find OpenDaVINCI.
find_package(OpenDaVINCI REQUIRED)

###########################################################################
# Find CxxTest.
if("${CXXTEST_INCLUDE_DIR}" STREQUAL "")
  message(STATUS "No CXXTEST_INCLUDE_DIR was supplied, using the default (./thirdparty/cxxtest) directory.")
  set(CXXTEST_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/cxxtest) 
endif()
include(CheckCxxTestEnvironment REQUIRED)

###########################################################################
# Global project variables.
set(INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
message(STATUS "Installing Chalmers Revere Scaled Cars to ${INSTALL_DIR}.")
set(PROJECT_ROOT ${CMAKE_SOURCE_DIR})

###########################################################################
###########################################################################
###########################################################################
# This "external project" simply deletes the existing libodvdcarolocupdatamodel.
ExternalProject_Add (libodvdcarolocupdatamodel-cleanup
    DOWNLOAD_COMMAND ""
    UPDATE_COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/libodvdcarolocupdatamodel
    PATCH_COMMAND ""
    SOURCE_DIR ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    TEST_COMMAND ""
    INSTALL_COMMAND "")

# This "external project" builds libodvdcarolocupdatamodel.
ExternalProject_Add (libodvdcarolocupdatamodel
    DEPENDS libodvdcarolocupdatamodel-cleanup
    DOWNLOAD_COMMAND ""
    UPDATE_COMMAND cd ${CMAKE_CURRENT_BINARY_DIR} && ${Java_JAVA_EXECUTABLE} -jar ${ODDATASTRUCTUREGENERATOR_EXECUTABLE} --withCMake ${CMAKE_CURRENT_SOURCE_DIR}/resources/odvd/ODVDCaroloCupDataModel.odvd
    PATCH_COMMAND ""
    SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/libodvdcarolocupdatamodel"
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}"
    CMAKE_ARGS "-DOPENDAVINCI_DIR=${OPENDAVINCI_DIR}"
    CMAKE_ARGS "-DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE}"
    CMAKE_ARGS "-DCXXTEST_INCLUDE_DIR=${CXXTEST_INCLUDE_DIR}"
    TEST_BEFORE_INSTALL 1
    TEST_COMMAND ${CMAKE_CTEST_COMMAND} ${CTEST_PARAMETERS}
    INSTALL_COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/libodvdcarolocupdatamodel-prefix/src/libodvdcarolocupdatamodel-build --target install COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/libodvdcarolocupdatamodel-prefix/src/libodvdcarolocupdatamodel-build --target ${MAKE_PACKAGE})

###############################################################################
# This "external project" builds and installs the system software stack.
ExternalProject_Add(code
    DEPENDS libodvdcarolocupdatamodel
    DOWNLOAD_COMMAND ""
    UPDATE_COMMAND ""
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/code"
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}"
    CMAKE_ARGS "-DOPENDAVINCI_DIR=${OPENDAVINCI_DIR}"
    CMAKE_ARGS "-DCXXTEST_INCLUDE_DIR=${CXXTEST_INCLUDE_DIR}"
    TEST_BEFORE_INSTALL 1
    INSTALL_COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/code-prefix/src/code-build --target install COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/code-prefix/src/code-build --target ${MAKE_PACKAGE})

