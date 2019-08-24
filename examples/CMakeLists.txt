add_subdirectory(CmdLine)
add_subdirectory(Desktop)

add_subdirectory(Render2DExample)

if(DJV_PYTHON)
    add_subdirectory(CmdLinePy)
    add_subdirectory(DesktopPy)
endif()

