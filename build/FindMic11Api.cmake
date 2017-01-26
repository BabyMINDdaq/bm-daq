# - Try to find Mic11api
# Once done this will define
#
#  MIC11API_FOUND - system has Mic11 API
#  MIC11API_INCLUDE_DIR - the Mic11 API include directory
#  MIC11API_LIBRARY - The libraries needed to use the Mic11 API
#

MESSAGE(" Looking for mic11api...")

find_path( MIC11API_INCLUDE_DIR NAMES Fifo.h   PATHS ${CMAKE_SOURCE_DIR}/../mic11api/src/)
find_library( MIC11API_LIBRARY NAMES mic11api  PATHS ${CMAKE_SOURCE_DIR}/../mic11api/lib/)

if (MIC11API_LIBRARY AND MIC11API_INCLUDE_DIR)

  set(MIC11API_FOUND TRUE)

  message(STATUS "found in  ${MIC11API_LIBRARY}\n")

else (MIC11API_LIBRARY AND MIC11API_INCLUDE_DIR)

  message(FATAL_ERROR "Could NOT find API!\n")

endif (MIC11API_LIBRARY AND MIC11API_INCLUDE_DIR)