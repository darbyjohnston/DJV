cmake_minimum_required(VERSION 3.18)

project(DJV-third-party-tests)

enable_testing()

foreach(DEP ${DJV_THIRD_PARTY_DEPS})
    find_package(${DEP})
    if(${DEP}_FOUND)
        add_subdirectory(${DEP}Test)
    endif()
endforeach()
