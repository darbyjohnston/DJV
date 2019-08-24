set(examples
    HelloWorldDesktopExample.py)

foreach(example ${examples})
    file(COPY ${example} DESTINATION ${DJV_BUILD_DIR}/bin)
endforeach()

