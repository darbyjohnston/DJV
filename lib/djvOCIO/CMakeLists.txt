set(header
	OCIO.h
	OCIOInline.h
	OCIOSystem.h
	OCIOSystemInline.h)
set(source
	OCIO.cpp
	OCIOSystem.cpp)


if(NOT BUILD_SHARED_LIBS)
  add_compile_definitions(OpenColorIO_SKIP_IMPORTS)
endif()

add_library(djvOCIO ${header} ${source})

find_package(OCIO REQUIRED)
include_directories (${OCIO_INCLUDES})
set(LIBRARIES
    djvGL
    ${OCIO_LIBRARIES})
target_link_libraries(djvOCIO ${LIBRARIES})

set_target_properties(
    djvOCIO
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

#install(
#    TARGETS djvOCIO
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib)
#install(FILES ${header} DESTINATION include/djvOCIO)

