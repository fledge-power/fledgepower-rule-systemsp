cmake_minimum_required(VERSION 2.6.0)

# Set the plugin name to build
project(systemspr)

# Supported options:
# -DFLEDGE_INCLUDE
# -DFLEDGE_LIB
# -DFLEDGE_SRC
# -DFLEDGE_INSTALL
#
# If no -D options are given and FLEDGE_ROOT environment variable is set
# then Fledge libraries and header files are pulled from FLEDGE_ROOT path.

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (${CMAKE_BUILD_TYPE} STREQUAL Coverage)
  message("Coverage is going to be generated")
  enable_testing()
  add_subdirectory(tests)
  include(CodeCoverage)
  append_coverage_compiler_flags()
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O3 --coverage")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 --coverage")
  set(GCOVR_ADDITIONAL_ARGS "--exclude-unreachable-branches" "--exclude-throw-branches" )

  setup_target_for_coverage_gcovr_sonar(NAME "${PROJECT_NAME}_coverage_sonar"
                                      EXECUTABLE RunTests
                                      DEPENDENCIES RunTests
                                      BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
                                      EXCLUDE "tests/*"
  )    
  
  setup_target_for_coverage_gcovr_html(NAME "${PROJECT_NAME}_coverage_html"
                                       EXECUTABLE RunTests
                                       DEPENDENCIES RunTests
                                       BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
                                       EXCLUDE "tests/*"
  )
  message(STATUS "Using Fledge dev package includes2 " ${FLEDGE_INCLUDE})
else()
  message("Build without Coverage") 
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb")

# Set plugin type (south, north, filter, notificationDelivery, notificationRule)
set(PLUGIN_TYPE "notificationRule")

# Generation version header file
set_source_files_properties(version.h PROPERTIES GENERATED TRUE)
add_custom_command(
  OUTPUT version.h
  DEPENDS ${CMAKE_SOURCE_DIR}/VERSION
  COMMAND ${CMAKE_SOURCE_DIR}/mkversion ${CMAKE_SOURCE_DIR}
  COMMENT "Generating version header"
  VERBATIM
)
include_directories(${CMAKE_BINARY_DIR})

# Add here all needed Fledge libraries as list
set(NEEDED_FLEDGE_LIBS common-lib plugins-common-lib)

# Find source files
file(GLOB SOURCES src/*.cpp)

# Find Fledge includes and libs, by including FindFledge.cmak file
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
find_package(Fledge)
# If errors: make clean and remove Makefile
if (NOT FLEDGE_FOUND)
	if (EXISTS "${CMAKE_BINARY_DIR}/Makefile")
		execute_process(COMMAND make clean WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
		file(REMOVE "${CMAKE_BINARY_DIR}/Makefile")
	endif()
	# Stop the build process
	message(FATAL_ERROR "Fledge plugin '${PROJECT_NAME}' build error.")
endif()
# On success, FLEDGE_INCLUDE_DIRS and FLEDGE_LIB_DIRS variables are set 

# Add ./include
include_directories(include)

# Add Fledge include dir(s)
include_directories(${FLEDGE_INCLUDE_DIRS})

# Add other include paths this plugin needs
if (FLEDGE_SRC)
	message(STATUS "Using third-party includes " ${FLEDGE_SRC}/C/thirdparty/Simple-Web-Server)
	include_directories(${FLEDGE_SRC}/C/thirdparty/Simple-Web-Server)
else()
	include_directories(${FLEDGE_INCLUDE_DIRS}/Simple-Web-Server)
endif()

# Add Fledge lib path
link_directories(${FLEDGE_LIB_DIRS})

# Create shared library
add_library(${PROJECT_NAME} SHARED ${SOURCES} version.h)

# Add Fledge library names
target_link_libraries(${PROJECT_NAME} ${NEEDED_FLEDGE_LIBS})
# Add additional libraries

# Set the build version 
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION 1)

set(FLEDGE_INSTALL "" CACHE INTERNAL "")
# Install library
if (FLEDGE_INSTALL)
	message(STATUS "Installing ${PROJECT_NAME} in ${FLEDGE_INSTALL}/plugins/${PLUGIN_TYPE}/${PROJECT_NAME}")
	install(TARGETS ${PROJECT_NAME} DESTINATION ${FLEDGE_INSTALL}/plugins/${PLUGIN_TYPE}/${PROJECT_NAME})
endif()
