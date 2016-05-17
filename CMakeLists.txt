# Copyright: (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
# Author: Giulia Vezzani <giulia.vezzani@iit.it>
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 2.8)

project(how-to-document-modules)

find_package(YARP REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
include(YarpIDL)
include(YarpInstallationHelpers)

find_package(ICUB REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${ICUB_MODULE_PATH})

find_package(ICUBcontrib)
list(APPEND CMAKE_MODULE_PATH ${ICUBCONTRIB_MODULE_PATH})
include(ICUBcontribHelpers)
include(ICUBcontribOptions)
icubcontrib_set_default_prefix()

# set your files
set(sources src/main.cpp)
# to allow thrift generation
yarp_add_idl(IDL_GEN_FILES src/idl.thrift)
source_group("IDL Files" FILES src/idl.thrift)
source_group("DOC Files" FILES ${PROJECT_NAME}.xml)
source_group("Source Files" FILES ${sources})

include_directories(${YARP_INCLUDE_DIRS} ${ICUB_INCLUDE_DIRS})

add_definitions(${YARP_DEFINES} -D_USE_MATH_DEFINES)

add_executable(${PROJECT_NAME} ${sources} src/idl.thrift ${PROJECT_NAME}.xml ${IDL_GEN_FILES})
target_link_libraries(${PROJECT_NAME} ${YARP_LIBRARIES})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
icubcontrib_add_uninstall_target()

# app and conf
file(GLOB conf ${CMAKE_CURRENT_SOURCE_DIR}/app/conf/*.ini)
yarp_install(FILES ${conf} DESTINATION ${ICUBCONTRIB_CONTEXTS_INSTALL_DIR}/${PROJECT_NAME})
file(GLOB scripts ${CMAKE_CURRENT_SOURCE_DIR}/app/scripts/*.template)
yarp_install(FILES ${scripts} DESTINATION ${ICUBCONTRIB_APPLICATIONS_TEMPLATES_INSTALL_DIR})
yarp_install(FILES ${PROJECT_NAME}.xml DESTINATION ${ICUBCONTRIB_MODULES_INSTALL_DIR})
