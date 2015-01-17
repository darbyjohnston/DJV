set(header
    djvAbstractTest.h
    djvTestLibExport.h)

set(mocHeader
    djvAbstractTest.h)

set(source
    djvTestLib.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvTestLib
    ${djvCoreIncludeDirs})

add_library(djvTestLib ${header} ${source} ${mocSource})

target_link_libraries(djvTestLib ${djvCoreDeps})

set_target_properties(djvTestLib PROPERTIES FOLDER tests)

if (djvInstallRpath)

    set_target_properties(
        djvTestLib PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    FILES ${header}
    DESTINATION include)

install(
    TARGETS djvTestLib
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

