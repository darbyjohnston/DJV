set(HEADERS)
set(SOURCE)
set(RESOURCES
    etc/Icons/DJV_Icon_512.svg
    etc/Icons/DJV_Icon_256.svg
    etc/Icons/DJV_Icon_128.svg
    etc/Icons/DJV_Icon_64.svg
    etc/Icons/DJV_Icon_32.svg
    etc/Icons/DJV_Icon_16.svg)
foreach(RESOURCE ${RESOURCES})
    get_filename_component(RESOURCE_BASE ${RESOURCE} NAME_WE)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE_BASE}.cpp
        COMMAND $<TARGET_FILE:feather-tk::feather-tk-res> ${CMAKE_SOURCE_DIR}/${RESOURCE} ${RESOURCE_BASE} djv_resource
        DEPENDS ${CMAKE_SOURCE_DIR}/${RESOURCE})
    list(APPEND SOURCE ${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE_BASE}.cpp)
endforeach()

add_library(djvResource ${HEADERS} ${SOURCE})
target_link_libraries(djvResource)
set_target_properties(djvResource PROPERTIES FOLDER lib)
set_target_properties(djvResource PROPERTIES PUBLIC_HEADER "${HEADERS}")

if(BUILD_SHARED_LIBS)
    install(
        TARGETS djvResource
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
endif()
