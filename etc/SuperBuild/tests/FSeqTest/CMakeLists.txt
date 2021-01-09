find_package(FSeq REQUIRED)

set(source main.cpp)

add_executable(FSeqTest ${header} ${source})
target_link_libraries(FSeqTest FSeq)

add_test(FSeqTest FSeqTest)
