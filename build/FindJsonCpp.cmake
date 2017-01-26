# Find JsonCpp
# This module finds an installed JsonCpp package.
#
# It sets the following variables:
#  JSONCPP_INCLUDE_DIR, where to find header, etc.
#  JSONCPP_LIBRARIE, the libraries needed to use jsoncpp.
#  JSONCPP_FOUND, If false, do not try to use jsoncpp.

MESSAGE(" Looking for jsoncpp ...")

find_path( JSONCPP_INCLUDE_DIR NAMES jsoncpp/json/json.h)
find_library( JSONCPP_LIBRARY NAMES jsoncpp)

# MESSAGE(STATUS "JSONCPP_INCLUDE_DIR: ${JSONCPP_INCLUDE_DIR}")
# MESSAGE(STATUS "JSONCPP_LIBRARY: ${JSONCPP_LIBRARY}")

IF (JSONCPP_INCLUDE_DIR AND JSONCPP_LIBRARY)

  SET(JSONCPP_FOUND TRUE)

ENDIF (JSONCPP_INCLUDE_DIR AND JSONCPP_LIBRARY)

IF (JSONCPP_FOUND)

  MESSAGE(STATUS "Found jsoncpp: ${JSONCPP_LIBRARY}\n")

ELSE (JSONCPP_FOUND)

  MESSAGE(FATAL_ERROR "Could not find jsoncpp!\n")

ENDIF (JSONCPP_FOUND)


