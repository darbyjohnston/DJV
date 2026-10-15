set(HEADERS)
set(SOURCE main.cpp)
if (WIN32)
    list(APPEND HEADERS winrc.h)
    list(APPEND SOURCE win.rc)
endif()

set(DJV_BIN djv)

add_executable(${DJV_BIN} WIN32 ${HEADERS} ${SOURCE})
target_link_libraries(${DJV_BIN} djvApp)
set_target_properties(${DJV_BIN} PROPERTIES FOLDER bin)

if(APPLE AND DJV_MACOS_PACKAGE)
    # A real application bundle, built here rather than assembled by the
    # packaging: CMake lays out "djv.app/Contents" itself, so nothing
    # downstream installs into a directory above the prefix to reach it, and
    # the executable gets its install rpath rewritten on the way in.
    # The USD document types appear only when this build reads them, so a
    # package without USD does not claim files it cannot open.
    set(DJV_MACOS_USD_DOCUMENT_TYPES)
    if(TLRENDER_USD)
        set(DJV_MACOS_USD_DOCUMENT_TYPES
"        <dict>
            <key>CFBundleTypeExtensions</key>
            <array>
                <string>usd</string>
                <string>usda</string>
                <string>usdc</string>
                <string>usdz</string>
            </array>
        </dict>
")
    endif()
    # The oldest macOS the application runs on, so that Finder says so on an
    # older one instead of the application failing to open: the deployment
    # target the build was made for, and on Apple silicon no older than 11,
    # the first release there. A build that names no target is for the
    # system doing the building.
    set(DJV_MACOS_MINIMUM "${CMAKE_OSX_DEPLOYMENT_TARGET}")
    if(NOT DJV_MACOS_MINIMUM)
        execute_process(
            COMMAND sw_vers -productVersion
            OUTPUT_VARIABLE DJV_MACOS_MINIMUM
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif()
    if(CMAKE_OSX_ARCHITECTURES STREQUAL "arm64" OR
        (NOT CMAKE_OSX_ARCHITECTURES AND CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64"))
        if(DJV_MACOS_MINIMUM VERSION_LESS 11)
            set(DJV_MACOS_MINIMUM 11.0)
        endif()
    endif()
    configure_file(
        ${PROJECT_SOURCE_DIR}/etc/macOS/Info.plist.in
        ${CMAKE_CURRENT_BINARY_DIR}/djv.plist)
    set_target_properties(
        ${DJV_BIN} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/djv.plist)

    install(
        TARGETS ${DJV_BIN}
        BUNDLE DESTINATION .
        COMPONENT bundle
        EXCLUDE_FROM_ALL)
else()
    install(TARGETS ${DJV_BIN} DESTINATION bin)
endif()

if(WIN32)
    add_executable(djv. ${HEADERS} ${SOURCE})
    target_link_libraries(djv. djvApp)
    set_target_properties(
        djv. PROPERTIES
        SUFFIX com
        FOLDER bin)
    install(TARGETS djv. RUNTIME DESTINATION bin)
endif()
