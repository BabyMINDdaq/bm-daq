MESSAGE(" src ...")

# Get a list of all source files in this directory.
AUX_SOURCE_DIRECTORY(.  SOURCE_FILES)


# set (bmdaq-guiLib_hdr BMDGui.h BMDCmdServer.h BMDCmdClient.h)
set (bmdaq-guiLib_hdr BMDGui.h)
set (bmdaq-guiLib_ui  mainwindow.ui)

qt5_wrap_cpp(bmdaq-guiLib_hdr_moc ${bmdaq-guiLib_hdr})
qt5_wrap_ui (bmdaq-guiLib_ui_moc  ${bmdaq-guiLib_ui})

if (_STATIC)

  MESSAGE(STATUS "building static library\n")
  add_library(bmdaq           ${SOURCE_FILES}
                              ${bmdaq-guiLib_hdr_moc}
                              ${bmdaq-guiLib_ui_moc})

else (_STATIC)

  MESSAGE(STATUS "building shered library\n")
  add_library(bmdaq SHARED    ${SOURCE_FILES}
                              ${bmdaq-guiLib_hdr_moc}
                              ${bmdaq-guiLib_ui_moc})

endif()

target_link_libraries(bmdaq   ${UFEC_LIBRARY}
                              ${UFECPP_LIBRARY}
                              ${MIC11API_LIBRARY}
                              ${BMUNPACK_LIBRARY}
                              ${JSONCPP_LIBRARY}
                              ${ZMQ_LIBRARY}
#                               Qt5::Network
                              Qt5::Widgets)

