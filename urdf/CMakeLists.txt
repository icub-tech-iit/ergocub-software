# Copyright (C) 2006-2022 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if( ERGOCUB_MODEL_GENERATE_SIMMECHANICS )
  set(BUILD_PREFIX "ergoCub")
  add_subdirectory(simmechanics)
  add_custom_target(generate-models ALL)

  if(TARGET generate-models-simmechanics)
    add_dependencies(generate-models generate-models-simmechanics)
    add_custom_target(copy-models-to-src ALL)
    add_dependencies(copy-models-to-src generate-models)
    add_custom_command(TARGET copy-models-to-src
                       POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/${BUILD_PREFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_PREFIX}"
                       COMMENT "Copying generated files to ${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_PREFIX}")
  endif()
endif()
