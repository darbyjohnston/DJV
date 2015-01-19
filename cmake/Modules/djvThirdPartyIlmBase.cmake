if (WIN32)

    add_definitions(-DPLATFORM_WINDOWS)
    add_definitions(-DOPENEXR_DLL)

endif (WIN32)

find_package(IlmBase REQUIRED)

#message("ILMBASE_LIBRARIES = " ${ILMBASE_LIBRARIES})
#message("ILMBASE_INCLUDE_DIRS = " ${ILMBASE_INCLUDE_DIRS})
    
if (djvPackageThirdParty)

    if (WIN32)

		install(
			FILES
            $ENV{CMAKE_PREFIX_PATH}/bin/Half.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Iex.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/IlmThread.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Imath.dll
			DESTINATION bin)

    elseif (APPLE)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/lib/libHalf.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libHalf.12.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libIex.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libIex-2_2.12.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libIexMath.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libIexMath-2_2.12.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libIlmThread.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libIlmThread-2_2.12.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libImath.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libImath-2_2.12.dylib
            DESTINATION lib)

    elseif (UNIX)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/lib/libHalf.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libHalf.so.12
            $ENV{CMAKE_PREFIX_PATH}/lib/libHalf.so.12.0.0
            $ENV{CMAKE_PREFIX_PATH}/lib/libIex.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libIex-2_2.so.12
            $ENV{CMAKE_PREFIX_PATH}/lib/libIex-2_2.so.12.0.0
            $ENV{CMAKE_PREFIX_PATH}/lib/libIexMath.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libIexMath-2_2.so.12
            $ENV{CMAKE_PREFIX_PATH}/lib/libIexMath-2_2.so.12.0.0
            $ENV{CMAKE_PREFIX_PATH}/lib/libIlmThread.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libIlmThread-2_2.so.12
            $ENV{CMAKE_PREFIX_PATH}/lib/libIlmThread-2_2.so.12.0.0
            $ENV{CMAKE_PREFIX_PATH}/lib/libImath.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libImath-2_2.so.12
            $ENV{CMAKE_PREFIX_PATH}/lib/libImath-2_2.so.12.0.0
            DESTINATION lib)

    endif (WIN32)

endif (djvPackageThirdParty)

