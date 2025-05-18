set(HEADERS)
set(SOURCE main.cpp)
if (WIN32)
    list(APPEND HEADERS winrc.h)
    list(APPEND SOURCE win.rc)
endif()

add_executable(djv WIN32 ${HEADER} ${SOURCE})
target_link_libraries(djv djvApp)
set_target_properties(djv PROPERTIES FOLDER bin)

install(
    TARGETS djv
    RUNTIME DESTINATION bin)

if(WIN32)
    add_executable(djv. ${HEADER} ${SOURCE})
    target_link_libraries(djv. djvApp)    
    set_target_properties(
        djv. PROPERTIES
        SUFFIX com
        FOLDER bin)
    install(
        TARGETS djv.
        RUNTIME DESTINATION bin)
endif()
