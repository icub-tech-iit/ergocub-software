# Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_configure_plugins_installation(ergocub-software)

# The committed versions of the param parsers is not compatible with YARP 3.12.0, so depending on the YARP version we set
# the option to automatically run the generator to ON if we detect an incompatible YARP version. This can be changed when
# compatibility with YARP 3.11 is dropped and YARP 3.12 compatible param parser are generated
if(YARP_VERSION VERSION_LESS 3.11.100)
  set(ALLOW_DEVICE_PARAM_PARSER_GENERATION_DEFAULT_VALUE OFF)
else()
  set(ALLOW_DEVICE_PARAM_PARSER_GENERATION_DEFAULT_VALUE ON)
endif()
# This must be before the call to yarp_prepare_plugin, otherwise the default value specified there
# will override the default value specified here
option(ALLOW_DEVICE_PARAM_PARSER_GENERATION "Generate the param parser for couplingXCubHandMk5 device" ${ALLOW_DEVICE_PARAM_PARSER_GENERATION_DEFAULT_VALUE})

yarp_prepare_plugin(couplingXCubHandMk5
                    CATEGORY device
                    TYPE CouplingXCubHandMk5
                    INCLUDE CouplingXCubHandMk5.h
                    GENERATE_PARSER
                    DEFAULT ON)

if(ENABLE_couplingXCubHandMk5)
  yarp_add_plugin(yarp_couplingXCubHandMk5)

  if(MSVC)
    add_definitions(-D_USE_MATH_DEFINES)
  endif()

  target_sources(yarp_couplingXCubHandMk5 PRIVATE CouplingXCubHandMk5.cpp
                                                  CouplingXCubHandMk5.h
                                                  CouplingXCubHandMk5_ParamsParser.cpp
                                                  CouplingXCubHandMk5_ParamsParser.h)

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
