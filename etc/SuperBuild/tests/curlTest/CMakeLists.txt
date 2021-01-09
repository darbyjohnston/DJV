find_package(curl REQUIRED)

set(source main.cpp)

add_executable(curlTest ${header} ${source})
target_link_libraries(curlTest curl)

add_test(curlTest curlTest)
