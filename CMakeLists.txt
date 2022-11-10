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

add_subdirectory(urdf)

# TODO to be defined
#if (BUILD_TESTING)
#  include( CTest )
#  enable_testing()
#  add_subdirectory(tests)
#endif()
