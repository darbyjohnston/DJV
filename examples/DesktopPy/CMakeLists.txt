set(examples
    HelloWorldDesktopExample.py)

foreach(examples ${examples})
    file(COPY ${example} DESTINATION ${DJV_BUILD_DIR}/bin)
endforeach()

