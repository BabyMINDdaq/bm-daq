# Find libufec
# This module finds an installed libufec package.
#
# It sets the following variables:
#  UFEC_INCLUDE_DIR, where to find header, etc.
#  UFEC_LIBRARIE, the libraries needed to use libufec.
#  UFEC_FOUND, If false, do not try to use libufec.

MESSAGE(" Looking for libufec ...")

find_path( UFEC_INCLUDE_DIR NAMES libufe.h   PATHS ${CMAKE_SOURCE_DIR}/../libufec/src/)
find_library( UFEC_LIBRARY NAMES ufec  PATHS ${CMAKE_SOURCE_DIR}/../libufec/lib/)

IF (UFEC_INCLUDE_DIR AND UFEC_LIBRARY)

  SET(UFEC_FOUND TRUE)

ENDIF (UFEC_INCLUDE_DIR AND UFEC_LIBRARY)

IF (UFEC_FOUND)

  MESSAGE(STATUS "Found libufec: ${UFEC_LIBRARY}\n")

ELSE (UFEC_FOUND)

  MESSAGE(FATAL_ERROR "Could not find libufec!\n")

ENDIF (UFEC_FOUND)