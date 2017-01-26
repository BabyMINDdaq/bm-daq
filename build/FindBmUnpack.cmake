# Find bmunpack
# This module finds an installed bmunpack package.
#
# It sets the following variables:
#  BMUNPACK_INCLUDE_DIR, where to find header, etc.
#  BMUNPACK_LIBRARIE, the libraries needed to use bmunpack.
#  BMUNPACK_FOUND, If false, do not try to use bmunpack.

MESSAGE(" Looking for bmunpack ...")

find_path( BMUNPACK_INCLUDE_DIR NAMES MDfragmentBM.h   PATHS ${CMAKE_SOURCE_DIR}/../bm-unpack/src/)
find_library( BMUNPACK_LIBRARY NAMES bmunpack  PATHS ${CMAKE_SOURCE_DIR}/../bm-unpack/lib/)

IF (BMUNPACK_INCLUDE_DIR AND BMUNPACK_LIBRARY)

  SET(BMUNPACK_FOUND TRUE)

ENDIF (BMUNPACK_INCLUDE_DIR AND BMUNPACK_LIBRARY)

IF (BMUNPACK_FOUND)

  MESSAGE(STATUS "Found bmunpack: ${BMUNPACK_LIBRARY}\n")

ELSE (BMUNPACK_FOUND)

  MESSAGE(FATAL_ERROR "Could not find bmunpack!\n")

ENDIF (BMUNPACK_FOUND)