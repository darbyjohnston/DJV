find_package(rkcommon REQUIRED)

set(source main.cpp)

add_executable(rkcommonTest ${header} ${source})
target_link_libraries(rkcommonTest rkcommon)

add_test(rkcommonTest rkcommonTest)
