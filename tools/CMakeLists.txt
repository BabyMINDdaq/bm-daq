MESSAGE(" tools ...")

MESSAGE(STATUS "bm-daq-ctrl")
add_executable (bm-daq-ctrl bm_daq.cpp)
target_link_libraries(bm-daq-ctrl bmdaq)

MESSAGE("")