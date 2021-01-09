find_package(OCIO REQUIRED)

set(source main.cpp)

add_executable(OCIOTest ${header} ${source})
target_link_libraries(OCIOTest OCIO)

add_test(OCIOTest OCIOTest)
