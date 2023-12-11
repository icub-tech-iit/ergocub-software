# Copyright (C) 2006-2022 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/conf/config.ini DESTINATION ${YARP_CONTEXTS_INSTALL_DIR}/${PROJECT_NAME})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/scripts/emotionHandler.xml DESTINATION ${YARP_APPLICATIONS_INSTALL_DIR}/${PROJECT_NAME})