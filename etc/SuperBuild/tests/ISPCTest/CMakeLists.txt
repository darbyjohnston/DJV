find_package(ISPC REQUIRED)

add_test(
    NAME ISPCTest
    COMMAND ${ISPC_EXECUTABLE} --version)
