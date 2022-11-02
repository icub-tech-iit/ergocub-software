# Copyright (C) 2006-2022 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)

project(ergocub-software)

# Give error if add_dependencies is called on a non-existing target
if(POLICY CMP0046)
  cmake_policy(SET CMP0046 NEW)
endif()

option(ERGOCUB_MODEL_GENERATE_SIMMECHANICS "Generate models using the model generation pipeline from simmechanics" ON)
option(BUILD_TESTING "Run tests for the generated models" OFF)

set(BUILD_PREFIX "ergoCub")

add_subdirectory(urdf)

add_custom_target(generate-models ALL)

if(TARGET generate-models-simmechanics)
  add_dependencies(generate-models generate-models-simmechanics)
endif()

# Logic to copy the generated models to icub-models
# We need to have the location of a YARP source dir
set(ICUB_MODELS_SOURCE_DIR "" CACHE STRING "Location of the icub-models repository source code")

add_custom_target(copy-models-to-src)
add_dependencies(copy-models-to-src generate-models)
add_custom_command(TARGET copy-models-to-src
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/${BUILD_PREFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/urdf/ergoCub"
                   COMMENT "Copying generated files to ${CMAKE_CURRENT_SOURCE_DIR}/urdf/ergoCub")


if (BUILD_TESTING)
  include( CTest )
  enable_testing()
  add_subdirectory(tests)
endif()