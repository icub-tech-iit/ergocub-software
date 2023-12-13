# Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

find_package(YARP 3.9.0 REQUIRED)

yarp_configure_plugins_installation(ergocub-software)

yarp_prepare_plugin(couplingXCubHandMk5
                    CATEGORY device
                    TYPE CouplingXCubHandMk5
                    INCLUDE CouplingXCubHandMk5.h
                    DEFAULT OFF)

if(ENABLE_couplingXCubHandMk5)

  yarp_add_plugin(yarp_couplingXCubHandMk5)

  if(MSVC)
    add_definitions(-D_USE_MATH_DEFINES)
  endif()

  target_sources(yarp_couplingXCubHandMk5 PRIVATE CouplingXCubHandMk5.cpp
                                                  CouplingXCubHandMk5.h)

  target_link_libraries(yarp_couplingXCubHandMk5 PRIVATE YARP::YARP_os
                                                         YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_dev)

  yarp_install(TARGETS yarp_couplingXCubHandMk5
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_couplingXCubHandMk5 PROPERTY FOLDER "Plugins/Device")
endif()
