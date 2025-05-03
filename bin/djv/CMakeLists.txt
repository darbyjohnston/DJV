add_executable(djv WIN32 main.cpp)
target_link_libraries(djv djvApp)
set_target_properties(djv PROPERTIES FOLDER bin)

install(
    TARGETS djv
    RUNTIME DESTINATION bin)

if(WIN32)
    add_executable(djv. main.cpp)
    target_link_libraries(djv. djvApp)    
    set_target_properties(
        djv. PROPERTIES
        SUFFIX com
        FOLDER bin)
    install(
        TARGETS djv.
        RUNTIME DESTINATION bin)
endif()
