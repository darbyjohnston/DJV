set(header
    djvAbstractTest.h
    djvTestLibExport.h)

set(source)

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvTestLib
    ${djvCoreIncludeDirs})

add_library(djvTestLib ${header} ${source})

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

