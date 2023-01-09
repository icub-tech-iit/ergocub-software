# Copyright (C) 2006-2022 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.



set(GAZEBO_SUPPORTED_MODELS "")
list(APPEND GAZEBO_SUPPORTED_MODELS "ergoCubGazeboV1")


set(INSTALLED_URDF_MODELS "")

macro(SUBDIRLIST result curdir)
  file(GLOB children RELATIVE ${curdir} ${curdir}/*)
  set(dirlist "")
  foreach(child ${children})
    if(IS_DIRECTORY ${curdir}/${child})
      list(APPEND dirlist ${child})
    endif()
  endforeach()
  set(${result} ${dirlist})
endmacro()

if( ERGOCUB_MODEL_GENERATE_SIMMECHANICS )
  add_subdirectory(simmechanics)
  add_custom_target(generate-models ALL)

  if(TARGET generate-models-simmechanics AND ERGOCUB_MODEL_COPY_TO_SRC )
    add_dependencies(generate-models generate-models-simmechanics)
    add_custom_target(copy-models-to-src ALL)
    add_dependencies(copy-models-to-src generate-models)
    add_custom_command(TARGET copy-models-to-src
                       POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/${BUILD_PREFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_PREFIX}"
                       COMMENT "Copying generated files to ${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_PREFIX}")
  endif()
else()
  # Copy the ergoCub folder in the build tree
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_PREFIX} DESTINATION ${CMAKE_BINARY_DIR})
endif()

set(ERGOCUB_MODELS_SDF_VERSION "1.7")

SUBDIRLIST(ROBOTS_NAMES ${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_PREFIX}/robots)
foreach(ROBOT_DIRNAME ${ROBOTS_NAMES})
  set(ROBOT_NAME ${ROBOT_DIRNAME})
  set(ROBOT_MODEL_CONFIG_FILE "${CMAKE_BINARY_DIR}/${BUILD_PREFIX}/robots/${ROBOT_NAME}/model.config")
  set(ROBOT_FIXED_MODEL_FOLDER "${CMAKE_BINARY_DIR}/${BUILD_PREFIX}/robots/${ROBOT_NAME}_fixed")
  set(ROBOT_FIXED_MODEL_CONFIG_FILE "${ROBOT_FIXED_MODEL_FOLDER}/model.config")
  set(ROBOT_FIXED_MODEL_SDF_FILE "${ROBOT_FIXED_MODEL_FOLDER}/${ROBOT_NAME}_fixed.sdf")

  set(ROBOT_FEET_FIXED_MODEL_FOLDER "${CMAKE_BINARY_DIR}/${BUILD_PREFIX}/robots/${ROBOT_NAME}_feet_fixed")
  set(ROBOT_FEET_FIXED_MODEL_CONFIG_FILE "${ROBOT_FEET_FIXED_MODEL_FOLDER}/model.config")
  set(ROBOT_FEET_FIXED_MODEL_SDF_FILE "${ROBOT_FEET_FIXED_MODEL_FOLDER}/${ROBOT_NAME}_feet_fixed.sdf")

  list(APPEND INSTALLED_URDF_MODELS \"${ROBOT_NAME}\")
  # When copying in sources remove the _fixed models
  if(ERGOCUB_MODEL_COPY_TO_SRC)
    file(REMOVE ${ROBOT_MODEL_CONFIG_FILE})
    file(REMOVE_RECURSE ${ROBOT_FIXED_MODEL_FOLDER})
    file(REMOVE_RECURSE ${ROBOT_FEET_FIXED_MODEL_FOLDER})
  elseif(ROBOT_NAME IN_LIST GAZEBO_SUPPORTED_MODELS)

    set(ROBOT_NAME_CONFIG "${ROBOT_NAME} (no hands)")
    set(ROBOT_MODEL_CONFIG "model.urdf")
    set(ROBOT_MODEL_DESCRIPTION_CONFIG "Model for the ergoCub humanoid robot.")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/model.config.in
                   ${ROBOT_MODEL_CONFIG_FILE}
                   @ONLY)

    file(MAKE_DIRECTORY ${ROBOT_FIXED_MODEL_FOLDER})
    set(ROBOT_NAME_CONFIG "${ROBOT_NAME}_fixed (no hands)")
    set(ROBOT_MODEL_CONFIG "${ROBOT_NAME}_fixed.sdf")
    set(ROBOT_MODEL_DESCRIPTION_CONFIG "Model for the ergoCub humanoid robot, fixed on the air")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/model.config.in
                   ${ROBOT_FIXED_MODEL_CONFIG_FILE}
                   @ONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/fixed_model.sdf.in
                   ${ROBOT_FIXED_MODEL_SDF_FILE}
                   @ONLY)

    file(MAKE_DIRECTORY ${ROBOT_FEET_FIXED_MODEL_FOLDER})
    set(ROBOT_NAME_CONFIG "${ROBOT_NAME}_feet_fixed (no hands)")
    set(ROBOT_MODEL_CONFIG "${ROBOT_NAME}_feet_fixed.sdf")
    set(ROBOT_MODEL_DESCRIPTION_CONFIG "Model for the ergoCub humanoid robot, with the feet fixed on the ground.")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/model.config.in
                   ${ROBOT_FEET_FIXED_MODEL_CONFIG_FILE}
                   @ONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/feet_fixed_model.sdf.in
                   ${ROBOT_FEET_FIXED_MODEL_SDF_FILE}
                   @ONLY)
  endif()
endforeach()

# install the directory
install(DIRECTORY ${CMAKE_BINARY_DIR}/${BUILD_PREFIX} DESTINATION share)
