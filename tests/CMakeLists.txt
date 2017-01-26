message(" bm-daq-tests ...")

# get list of all source files in this directory.
AUX_SOURCE_DIRECTORY(.              TESTS_SOURCE_FILES)
# message(STATUS "TESTS_SOURCE_FILES " ${TESTS_SOURCE_FILES})

add_library (bmdaq-tests      ${TESTS_SOURCE_FILES})

target_link_libraries(bmdaq-tests  bmdaq cppunit)

ADD_EXECUTABLE(unit_test              UnitTests.cpp)
TARGET_LINK_LIBRARIES(unit_test       bmdaq-tests jsoncpp)

message("")
