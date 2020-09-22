set(header
	OCIO.h
	OCIOInline.h
	OCIOSystem.h
	OCIOSystemFunc.h
	OCIOSystemInline.h)
set(source
	OCIO.cpp
	OCIOSystem.cpp
	OCIOSystemFunc.cpp)

add_library(djvOCIO ${header} ${source})
set(LIBRARIES
    djvGL
    OCIO)
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

