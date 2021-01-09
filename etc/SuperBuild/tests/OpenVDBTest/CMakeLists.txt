find_package(OpenVDB REQUIRED)

set(source main.cpp)

add_executable(OpenVDBTest ${header} ${source})
target_link_libraries(OpenVDBTest OpenVDB)

add_test(OpenVDBTest OpenVDBTest)
