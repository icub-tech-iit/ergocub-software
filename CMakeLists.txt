# Copyright (C) 2006-2022 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)

project(ergocub-software
        VERSION 0.0.0)

# Give error if add_dependencies is called on a non-existing target
if(POLICY CMP0046)
  cmake_policy(SET CMP0046 NEW)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(AddUninstallTarget)

option(ERGOCUB_MODEL_GENERATE_SIMMECHANICS "Generate models using the model generation pipeline from simmechanics" OFF)
option(ERGOCUB_MODEL_COPY_TO_SRC "Copy to sources the generated urdf files" OFF)
option(BUILD_TESTING "Run tests for the generated models" OFF)

set(BUILD_PREFIX "ergoCub")
add_subdirectory(urdf)

# TODO to be defined
if (BUILD_TESTING)
  include( CTest )
  enable_testing()
  add_subdirectory(tests)
endif()
