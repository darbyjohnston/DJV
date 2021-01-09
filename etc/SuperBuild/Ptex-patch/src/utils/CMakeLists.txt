add_executable(ptxinfo ptxinfo.cpp)
add_definitions(-DPTEX_VER="${PTEX_VER} \(${PTEX_SHA}\)" -DPTEX_STATIC)

if(PTEX_BUILD_SHARED_LIBS)
    target_link_libraries(ptxinfo Ptex_dynamic ZLIB ${CMAKE_THREAD_LIBS_INIT})
elseif(PTEX_BUILD_STATIC_LIBS)
    target_link_libraries(ptxinfo Ptex_static ZLIB ${CMAKE_THREAD_LIBS_INIT})
endif()

install(TARGETS ptxinfo DESTINATION ${CMAKE_INSTALL_BINDIR})
