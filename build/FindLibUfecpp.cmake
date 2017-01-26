# Find libufe-cpp
# This module finds an installed libufe-cpp package.
#
# It sets the following variables:
#  UFECPP_INCLUDE_DIR, where to find header, etc.
#  UFECPP_LIBRARY, the libraries needed to use libufe-cpp.
#  UFECPP_FOUND, If false, do not try to use libufe-cpp.

MESSAGE(" Looking for libufe-cpp ...")

find_path( UFECPP_INCLUDE_DIR NAMES  UFEDeff.h PATHS ${CMAKE_SOURCE_DIR}/../libufecpp/src/)
find_library( UFECPP_LIBRARY NAMES ufe-cpp  PATHS ${CMAKE_SOURCE_DIR}/../libufecpp/lib/)

IF (UFECPP_INCLUDE_DIR AND UFECPP_LIBRARY)

  SET(UFECPP_FOUND TRUE)

ENDIF (UFECPP_INCLUDE_DIR AND UFECPP_LIBRARY)

IF (UFECPP_FOUND)

  MESSAGE(STATUS "Found libufe-cpp: ${UFECPP_LIBRARY}\n")

ELSE (UFECPP_FOUND)

  MESSAGE(FATAL_ERROR "Could not find libufe-cpp!\n")

ENDIF (UFECPP_FOUND)