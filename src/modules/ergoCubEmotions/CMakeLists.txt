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
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PUBLIC ergoCubEmotions.cpp ergoCubEmotions.h main.cpp)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/images DESTINATION ${CMAKE_BINARY_DIR})

# Link the application with YARP and OpenCV libraries
target_link_libraries(${PROJECT_NAME} YARP::YARP_os
                                      YARP::YARP_dev
                                      YARP::YARP_init
                                      opencv_core
                                      opencv_highgui
                                      opencv_videoio)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
