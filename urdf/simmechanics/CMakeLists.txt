# Copyright (C) 2006-2022 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

find_package(PythonInterp REQUIRED)

# Generate URDF models for
# v2.5 robots using the simmechanics-to-urdf script
macro(generate_ergocub_simmechanics)
    set(options INCREASE_INERTIA_FOR_GAZEBO
                ERGOCUB_COLLISION_GEOM_MOD)
    set(oneValueArgs YARP_ROBOT_NAME SIMMECHANICS_XML YAML_TEMPLATE CSV_TEMPLATE)
    set(multiValueArgs)
    cmake_parse_arguments(GIVTWO "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Fallback if no assigned collision geometry is requested
    set(ASSIGNED_COLLISION_GEOMETRIES "")

    # Generate the YAML file from the CMake configuration
    if(GIVTWO_INCREASE_INERTIA_FOR_GAZEBO)
      set(GAZEBO_ASSIGNED_INERTIAS
"assignedInertias:
  - linkName: r_hip_3
    xx: 0.01
    yy: 0.01
    zz: 0.01
  - linkName: r_ankle_2
    xx: 0.01
    yy: 0.01
    zz: 0.01
  - linkName: l_hip_3
    xx: 0.01
    yy: 0.01
    zz: 0.01
  - linkName: l_ankle_2
    xx: 0.01
    yy: 0.01
    zz: 0.01
  - linkName: r_shoulder_1
    xx: 0.01
    yy: 0.01
    zz: 0.01
  - linkName: r_shoulder_2
    xx: 0.01
    yy: 0.01
    zz: 0.01
  - linkName: r_shoulder_3
    xx: 0.01
    yy: 0.01
    zz: 0.01
  - linkName: r_elbow_1
    xx: 0.01
    yy: 0.01
    zz: 0.01
  - linkName: r_wrist_1
    xx: 0.01
    yy: 0.01
    zz: 0.01
  - linkName: r_hand
    xx: 0.01
    yy: 0.01
    zz: 0.01
  - linkName: l_shoulder_1
    xx: 0.01
    yy: 0.01
    zz: 0.01
  - linkName: l_shoulder_2
    xx: 0.01
    yy: 0.01
    zz: 0.01
  - linkName: l_shoulder_3
    xx: 0.01
    yy: 0.01
    zz: 0.01
  - linkName: l_forearm
    xx: 0.01
    yy: 0.01
    zz: 0.01
  - linkName: l_wrist_1
    xx: 0.01
    yy: 0.01
    zz: 0.01
  - linkName: l_hand
    xx: 0.01
    yy: 0.01
    zz: 0.01
  - linkName: neck_1
    xx: 0.01
    yy: 0.01
    zz: 0.01
  - linkName: neck_2
    xx: 0.01
    yy: 0.01
    zz: 0.01
")
      set(LOWERBODY_JOINT_DAMPING 1.0)
      set(UPPERBODY_JOINT_DAMPING 1.0)
      set(ASSIGNED_COLLISION_GEOMETRIES
"assignedCollisionGeometry:
- linkName: r_foot
  geometricShape:
    shape: box
    size: 0.16 0.072 0.001
    origin: \"0.03 0.005 0.014 0.0 0.0 0.0\"
- linkName: l_foot
  geometricShape:
    shape: box
    size: 0.16 0.072 0.001
    origin: \"0.03 -0.005 0.014 0.0 0.0 0.0\"
")
    else()
      set(GAZEBO_ASSIGNED_INERTIAS "")
      set(LOWERBODY_JOINT_DAMPING 0.223)
      set(UPPERBODY_JOINT_DAMPING 0.06)
    endif()

    if (GIVTWO_ERGOCUB_COLLISION_GEOM_MOD)
      set(ASSIGNED_COLLISION_GEOMETRIES
"assignedCollisionGeometry:
  - linkName: r_foot_front
    geometricShape:
      shape: box
      size: 0.117 0.100 0.006
      origin: \"0.0 0.0 0.003 0.0 0.0 0.0\"
  - linkName: r_foot_rear
    geometricShape:
      shape: box
      size: 0.117 0.100 0.006
      origin: \"0.0 0.0 0.003 0.0 0.0 0.0\"
  - linkName: l_foot_front
    geometricShape:
      shape: box
      size: 0.117 0.100 0.006
      origin: \"0.0 0.0 0.003 0.0 0.0 0.0\"
  - linkName: l_foot_rear
    geometricShape:
      shape: box
      size: 0.117 0.100 0.006
      origin: \"0.0 0.0 0.003 0.0 0.0 0.0\"
")
    endif()

    set(MESH_FILE_FORMAT "filenameformatchangeext: \"package://iCub/meshes/simmechanics/%s-binary.stl\"")
    set(ANKLE_PITCH_ROM "-35,35")
    set(CUSTOM_EPSILON "")
    set(XSENS_IMU_FRAME "")
    set(FT_IMU_SENSORS
"  - frameName: SCSYS_R_UPPER_LEG_FT-IMU_3B11
    linkName: r_hip_2
    sensorName: r_upper_leg_ft_acc_3b11
    sensorType: \"accelerometer\"
    sensorBlobs: \"\"
  - frameName: SCSYS_R_UPPER_LEG_FT-IMU_3B11
    linkName: r_hip_2
    sensorName: r_upper_leg_ft_gyro_3b11
    sensorType: \"gyroscope\"
    sensorBlobs: \"\"
  - frameName: SCSYS_R_FOOT_FT-IMU_3B14
    linkName: r_ankle_2
    sensorName: r_foot_ft_acc_3b14
    sensorType: \"accelerometer\"
    sensorBlobs: \"\"
  - frameName: SCSYS_R_FOOT_FT-IMU_3B14
    linkName: r_ankle_2
    sensorName: r_foot_ft_gyro_3b14
    sensorType: \"gyroscope\"
    sensorBlobs: \"\"
  - frameName: SCSYS_L_UPPER_LEG_FT-IMU_3B12
    linkName: l_hip_2
    sensorName: l_upper_leg_ft_acc_3b12
    sensorType: \"accelerometer\"
    sensorBlobs: \"\"
  - frameName: SCSYS_L_UPPER_LEG_FT-IMU_3B12
    linkName: l_hip_2
    sensorName: l_upper_leg_ft_gyro_3b12
    sensorType: \"gyroscope\"
    sensorBlobs: \"\"
  - frameName: SCSYS_L_FOOT_FT-IMU_3B13
    linkName: l_ankle_2
    sensorName: l_foot_ft_acc_3b13
    sensorType: \"accelerometer\"
    sensorBlobs: \"\"
  - frameName: SCSYS_L_FOOT_FT-IMU_3B13
    linkName: l_ankle_2
    sensorName: l_foot_ft_gyro_3b13
    sensorType: \"gyroscope\"
    sensorBlobs: \"\"
")
    set(XSENS_IMU_SENSOR "")
    set(REVERSE_ROTATION_AXIS
"reverseRotationAxis:
  l_shoulder_roll
  l_elbow
  l_hip_yaw
  l_knee
  l_ankle_pitch
  r_hip_pitch
  r_ankle_roll
  r_shoulder_pitch
  r_shoulder_yaw
  r_elbow
  r_wrist_prosup
  torso_pitch
  neck_roll
  r_wrist_yaw
")


    set(RFE_ADDITIONAL_TRANSFORMATION
         "additionalTransformation: [0.0323779, -0.0139537, 0.072, -3.14159265358979, 0, -1.5707963267949]")

    set(ADDITIONAL_XML_BLOBS "")


    set(GENERATED_YAML_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/${GIVTWO_YARP_ROBOT_NAME}.yaml)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/${GIVTWO_YAML_TEMPLATE}
                   ${GENERATED_YAML_LOCATION}
                   @ONLY)
    set(GENERATED_CSV_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/${GIVTWO_YARP_ROBOT_NAME}.csv)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/${GIVTWO_CSV_TEMPLATE}
                   ${GENERATED_CSV_LOCATION}
                   @ONLY)

    add_custom_command(OUTPUT ${GIVTWO_YARP_ROBOT_NAME}.urdf
                       COMMAND simmechanics_to_urdf
                       ARGS ${CMAKE_CURRENT_SOURCE_DIR}/data/${GIVTWO_SIMMECHANICS_XML}
                            --output xml
                            --yaml ${GENERATED_YAML_LOCATION}
                            --csv-joints ${GENERATED_CSV_LOCATION}
                            --outputfile ${GIVTWO_YARP_ROBOT_NAME}.urdf
                       MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/data/${GIVTWO_SIMMECHANICS_XML}"
                       DEPENDS  "${GENERATED_YAML_LOCATION}"
                                "${GENERATED_CSV_LOCATION}")

    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/${BUILD_PREFIX}/robots/${GIVTWO_YARP_ROBOT_NAME}/model.urdf"
                        MAIN_DEPENDENCY "${GIVTWO_YARP_ROBOT_NAME}.urdf"
                        COMMAND ${CMAKE_COMMAND} -E
                                copy "${GIVTWO_YARP_ROBOT_NAME}.urdf" "${CMAKE_BINARY_DIR}/${BUILD_PREFIX}/robots/${GIVTWO_YARP_ROBOT_NAME}/model.urdf")


    list(APPEND model-simmechanics-generated-models "${CMAKE_BINARY_DIR}/${BUILD_PREFIX}/robots/${GIVTWO_YARP_ROBOT_NAME}/model.urdf")
endmacro()

set(model-simmechanics-generated-models "")

generate_ergocub_simmechanics(YARP_ROBOT_NAME ergoCubGazeboV1
                           SIMMECHANICS_XML "ergocub/SIM_ERGOCUB_1-0.xml"
                           YAML_TEMPLATE "ergocub/ERGOCUB_all_options_gazebo.yaml.in"
                           CSV_TEMPLATE "ergocub/ERGOCUB_joint_all_parameters.csv"
                           ERGOCUB_COLLISION_GEOM_MOD)
generate_ergocub_simmechanics(YARP_ROBOT_NAME ergoCubSN000
                           SIMMECHANICS_XML "ergocub/SIM_ERGOCUB_1-0.xml"
                           YAML_TEMPLATE "ergocub/ERGOCUB_all_options.yaml"
                           CSV_TEMPLATE "ergocub/ERGOCUB_joint_all_parameters.csv")


add_custom_target(generate-models-simmechanics
                  ALL
                  DEPENDS ${model-simmechanics-generated-models})

# Copy the meshes in the meshes/simmechanics directory
add_custom_command(TARGET generate-models-simmechanics
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/data/ergocub/meshes" "${CMAKE_BINARY_DIR}/${BUILD_PREFIX}/meshes"
                   COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/data/ergocub/conf" "${CMAKE_BINARY_DIR}/${BUILD_PREFIX}/conf"
                   COMMENT "Copying Simmechanics meshes")

# Convert meshes from ASCII to binary
add_custom_command(TARGET generate-models-simmechanics
                   POST_BUILD
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                   COMMAND ruby ./scripts/convertSTL.rb ${CMAKE_BINARY_DIR}/${BUILD_PREFIX}/meshes/simmechanics/*.stl
                   COMMENT "Converting Simmechanics meshes to binary")
