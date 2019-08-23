set(examples
    AnimationCmdLineExample.py)

foreach(examples ${examples})
    file(COPY ${example} DESTINATION ${DJV_BUILD_DIR}/bin)
endforeach()

