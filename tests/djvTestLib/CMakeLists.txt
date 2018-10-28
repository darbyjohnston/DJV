set(header
    AbstractTest.h
    TestLib.h)
set(mocHeader
    AbstractTest.h)
set(source
    AbstractTest.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

add_library(djvTestLib ${header} ${source} ${mocSource})
target_link_libraries(djvTestLib djvCore)
set_target_properties(djvTestLib PROPERTIES FOLDER tests CXX_STANDARD 11)

install(
    TARGETS djvTestLib
    RUNTIME DESTINATION ${DJV_INSTALL_BIN}
    LIBRARY DESTINATION ${DJV_INSTALL_LIB}
    ARCHIVE DESTINATION ${DJV_INSTALL_LIB})

