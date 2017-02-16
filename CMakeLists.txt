# Check if cmake has the required version
cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

# Set name of our project
project(bm-daq)
MESSAGE("\n project: bm-daq \n")

set(BMDAQ_DIR ${CMAKE_SOURCE_DIR})

include(${CMAKE_BINARY_DIR}/FindLibUsb.cmake)
include(${CMAKE_BINARY_DIR}/FindLibUfec.cmake)
include(${CMAKE_BINARY_DIR}/FindLibUfecpp.cmake)
include(${CMAKE_BINARY_DIR}/FindJsonCpp.cmake)
include(${CMAKE_BINARY_DIR}/FindMic11Api.cmake)
include(${CMAKE_BINARY_DIR}/FindBmUnpack.cmake)
include(${CMAKE_BINARY_DIR}/FindZeroMQ.cmake)
include(${CMAKE_BINARY_DIR}/FindCppUnit.cmake)

find_package (Qt5Widgets REQUIRED)
# find_package (Qt5Network REQUIRED)

find_package(Doxygen)

configure_file( ${CMAKE_BINARY_DIR}/deff.h.cmake
                ${CMAKE_SOURCE_DIR}/src/BMDDeff.h)

set(LIBRARY_OUTPUT_PATH    "${CMAKE_SOURCE_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -Wall -O3 -DZMQ_ENABLE")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -O3")

if (VERBOSE GREATER 1)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUFE_INFO")
endif (VERBOSE GREATER 1)

if (VERBOSE GREATER 2)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUFE_DEBUGG")
endif (VERBOSE GREATER 2)

include_directories( ${CMAKE_SOURCE_DIR}/.
                     ${CMAKE_SOURCE_DIR}/src
                     ${CMAKE_BINARY_DIR}/src/
                     ${UFEC_INCLUDE_DIR}
                     ${UFECPP_INCLUDE_DIR}
                     ${JSONCPP_INCLUDE_DIR}
                     ${MIC11API_INCLUDE_DIR}
                     ${BMUNPACK_INCLUDE_DIR})


link_directories(     ${CMAKE_SOURCE_DIR}/lib
                      /usr/local/lib/)

message(STATUS "C flags      : " ${CMAKE_C_FLAGS})
message(STATUS "CXX flags    : " ${CMAKE_CXX_FLAGS})
message(STATUS "Linker flags : " ${CMAKE_EXE_LINKER_FLAGS}\n)

add_subdirectory(${CMAKE_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_SOURCE_DIR}/tools)

if    (CPPUNIT_FOUND)
  add_subdirectory(${CMAKE_SOURCE_DIR}/tests)
endif (CPPUNIT_FOUND)


if (_DOXYGEN_DOC AND DOXYGEN_FOUND)

  message(" doxygen documentation ...\n")
  add_custom_target(doc ALL)
  add_custom_command(TARGET doc
        COMMAND doxygen dox_config
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/doc)

endif ()
