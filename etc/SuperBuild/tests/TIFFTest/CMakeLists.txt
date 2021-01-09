find_package(TIFF REQUIRED)

set(source main.cpp)

add_executable(TIFFTest ${header} ${source})
target_link_libraries(TIFFTest TIFF)

add_test(TIFFTest TIFFTest)
