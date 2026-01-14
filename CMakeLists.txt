# Copyright (C) 2006-2022 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)

project(ergocub-software LANGUAGES C CXX
                         VERSION 0.7.11)

find_package(YARP COMPONENTS conf os sig dev math idl_tools REQUIRED)

# Give error if add_dependencies is called on a non-existing target
if(POLICY CMP0046)
  cmake_policy(SET CMP0046 NEW)
endif()

find_package(YCM 0.12 REQUIRED)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(AddUninstallTarget)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

option(BUILD_TESTING "Run tests for the generated models" OFF)
option(COMPILE_ergoCubEmotions "Compile the module controlling the ergoCub emotions" OFF)
option(COMPILE_ergoCubModels "Configure and install the URDF models of the ergoCub robots" ON)

if(MSVC)
  set(CMAKE_DEBUG_POSTFIX "d")
endif()

# To build shared libraries in Windows, we set CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS to TRUE.
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

option(BUILD_SHARED_LIBS "Build libraries as shared as opposed to static" ON)
# Build position independent code.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Enable RPATH support for installed binaries and libraries
include(AddInstallRPATHSupport)
add_install_rpath_support(LIB_DIRS "${CMAKE_INSTALL_FULL_LIBDIR}"       # Libraries
                          BIN_DIRS "${CMAKE_INSTALL_FULL_BINDIR}"       # Binaries
                                   "${CMAKE_INSTALL_FULL_LIBDIR}/yarp"  # Plugins
                          INSTALL_NAME_DIR "${CMAKE_INSTALL_FULL_LIBDIR}"
                          USE_LINK_PATH)

# Encourage user to specify a build type (e.g. Release, Debug, etc.), otherwise set it to Release.
if(NOT CMAKE_CONFIGURATION_TYPES)
    if(NOT CMAKE_BUILD_TYPE)
        message(STATUS "Setting build type to 'Release' as none was specified.")
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE "Release")
    endif()
endif()

set(BUILD_PREFIX "ergoCub")

if (COMPILE_ergoCubModels)
  add_subdirectory(urdf)
endif()

# TODO to be defined
if (BUILD_TESTING)
  include( CTest )
  enable_testing()
  add_subdirectory(tests)
endif()

add_subdirectory(app)
add_subdirectory(src)
