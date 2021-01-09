find_package(ospray REQUIRED)

set(source main.cpp)

add_executable(osprayTest ${header} ${source})
target_link_libraries(osprayTest ospray::ospray)

add_test(osprayTest osprayTest)
