add_subdirectory(Render2DStressTest)
add_subdirectory(djvAVTest)
add_subdirectory(djvCoreTest)
add_subdirectory(djvTest)
add_subdirectory(djvTestLib)

if(DJV_PYTHON)
	add_subdirectory(djvCorePyTest)
endif()
