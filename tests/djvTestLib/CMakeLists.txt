set(header
    Test.h)
set(mocHeader)
set(source
    Test.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library(djvTestLib ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(
    djvTestLib
    djvCore)
set_target_properties(
    djvTestLib
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)

