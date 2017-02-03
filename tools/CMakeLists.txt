MESSAGE(" tools ...")

MESSAGE(STATUS "bm-daq-local-ctrl")
add_executable (bm-daq-local-ctrl        bm_daq_local_ctrl.cpp)
target_link_libraries(bm-daq-local-ctrl  bmdaq)

MESSAGE(STATUS "bm-daq-remote-ctrl")
add_executable (bm-daq-remote-ctrl        bm_daq_remote_ctrl.cpp)
target_link_libraries(bm-daq-remote-ctrl  bmdaq
                                          ${UFEC_LIBRARY}
                                          ${ZMQ_LIBRARY})

MESSAGE(STATUS "bm-daq-gui")
add_executable (bm-daq-gui        bm_daq_gui.cpp)
target_link_libraries(bm-daq-gui  bmdaq)

MESSAGE("")