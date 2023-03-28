################################################################################
#                                                                              #
# Copyright (C) 2023 Fondazione Istitito Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

project(ergoCubEmotions)
cmake_minimum_required(VERSION 3.12)

find_package(YARP 3.7.2 REQUIRED)
find_package(OpenCV 4.5.4 REQUIRED)

set(doc_files ${PROJECT_NAME}.xml)
yarp_add_idl(IDL_GEN_FILES idl.thrift)

source_group("IDL Files" FILES idl.thrift)
source_group("DOC Files" FILES ${doc_files})
add_executable(${PROJECT_NAME} ergoCubEmotions.cpp ergoCubEmotions.h main.cpp idl.thrift ${IDL_GEN_FILES} ${doc_files})

install(FILES config.ini DESTINATION ${YARP_CONTEXTS_INSTALL_DIR}/${PROJECT_NAME})
install(DIRECTORY expressions DESTINATION ${YARP_CONTEXTS_INSTALL_DIR}/${PROJECT_NAME})

# Link the application with YARP and OpenCV libraries
target_link_libraries(${PROJECT_NAME} YARP::YARP_os
                                      YARP::YARP_dev
                                      YARP::YARP_init
                                      opencv_core
                                      opencv_highgui
                                      opencv_videoio)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
