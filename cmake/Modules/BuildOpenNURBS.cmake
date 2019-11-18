include(ExternalProject)

if(NOT DJV_THIRD_PARTY_DISABLE_BUILD)
    ExternalProject_Add(
        OpenNURBSThirdParty
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/OpenNURBS
        GIT_REPOSITORY https://github.com/mcneel/opennurbs.git
        GIT_TAG 0510f9da027ab5ccc3130cb1a7f9ebd46671409d
        PATCH_COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/third-party/opennurbs-patch/CMakeLists.txt
            ${CMAKE_CURRENT_BINARY_DIR}/OpenNURBS/src/OpenNURBSThirdParty/CMakeLists.txt
        CMAKE_ARGS
            -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
            -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
            -DBUILD_SHARED_LIBS=${OpenNURBS_SHARED_LIBS})
endif()

set(OpenNURBS_FOUND TRUE)
set(OpenNURBS_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)
set(OpenNURBS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/opennurbs${CMAKE_STATIC_LIBRARY_SUFFIX})
set(OpenNURBS_LIBRARIES ${OpenNURBS_LIBRARY})

if(OpenNURBS_FOUND AND NOT TARGET OpenNURBS::OpenNURBS)
    add_library(OpenNURBS::OpenNURBS UNKNOWN IMPORTED)
    add_dependencies(OpenNURBS::OpenNURBS OpenNURBSThirdParty)
    set_target_properties(OpenNURBS::OpenNURBS PROPERTIES
        IMPORTED_LOCATION "${OpenNURBS_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${OpenNURBS_INCLUDE_DIR}")
endif()
if(OpenNURBS_FOUND AND NOT TARGET OpenNURBS)
    add_library(OpenNURBS INTERFACE)
    target_link_libraries(OpenNURBS INTERFACE OpenNURBS::OpenNURBS)
endif()
