# Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

find_package(YARP 3.9.0 REQUIRED)

yarp_configure_plugins_installation(ergocub-software)

yarp_prepare_plugin(couplingHandMk5
                    CATEGORY device
                    TYPE HandMk5CouplingHandler
                    INCLUDE HandMk5CouplingHandler.h
                    DEFAULT OFF)

if(ENABLE_couplingHandMk5)

  yarp_add_plugin(yarp_couplingHandMk5)

  if(MSVC)
    add_definitions(-D_USE_MATH_DEFINES)
  endif()

  target_sources(yarp_couplingHandMk5 PRIVATE HandMk5CouplingHandler.cpp
                                                     HandMk5CouplingHandler.h)

  target_link_libraries(yarp_couplingHandMk5 PRIVATE YARP::YARP_os
                                                           YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_dev)

  yarp_install(TARGETS yarp_couplingHandMk5
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_couplingHandMk5 PROPERTY FOLDER "Plugins/Device")
endif()
