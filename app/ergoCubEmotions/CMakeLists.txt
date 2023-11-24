################################################################################
#                                                                              #
# Copyright (C) 2023 Fondazione Istitito Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

project(ergoCubEmotions)
cmake_minimum_required(VERSION 3.12)

find_package(YARP 3.7.2 REQUIRED)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/conf/config.ini DESTINATION ${YARP_CONTEXTS_INSTALL_DIR}/${PROJECT_NAME})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/scripts/emotionHandler.xml DESTINATION ${YARP_APPLICATIONS_INSTALL_DIR}/${PROJECT_NAME})