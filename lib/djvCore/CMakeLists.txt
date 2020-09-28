set(header
    Cache.h
    CacheInline.h
    Core.h
    Enum.h
    ErrorFunc.h
    ICommand.h
    ListObserver.h
    ListObserverInline.h
    MapObserver.h
    MapObserverInline.h
    Memory.h
    MemoryFunc.h
    MemoryFuncInline.h
    Namespace.h
    OSFunc.h
    OSFuncInline.h
    Observer.h
    RandomFunc.h
    RandomFuncInline.h
    RapidJSONFunc.h
    RapidJSONTemplates.h
    RapidJSONTemplatesInline.h
    StringFormat.h
    StringFormatInline.h
    StringFunc.h
    StringFuncInline.h
    String.h
    Time.h
    TimeFunc.h
    TimeFuncInline.h
    UID.h
    UIDFunc.h
    UndoStack.h
    ValueObserver.h
    ValueObserverInline.h)
set(source
    Core.cpp
    ErrorFunc.cpp
    ICommand.cpp
    MemoryFunc.cpp
    OSFunc.cpp
    RapidJSONFunc.cpp
    RandomFunc.cpp
    StringFormat.cpp
    StringFunc.cpp
    TimeFunc.cpp
    UIDFunc.cpp
    UndoStack.cpp)
if (WIN32)
    set(source
        ${source}
        ErrorFuncWin32.cpp
        OSFuncWin32.cpp
        TimeFuncWin32.cpp)
else()
    set(source
        ${source}
        OSFuncUnix.cpp
        TimeFuncUnix.cpp)
endif()

add_library(djvCore ${header} ${source})
set(LIBRARIES
    RapidJSON)
if (${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "arm")
    set(LIBRARIES ${LIBRARIES} atomic)
endif()
target_link_libraries(djvCore ${LIBRARIES})
set_target_properties(
    djvCore
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

#install(
#    TARGETS djvCore
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib)
#install(FILES ${header} DESTINATION include/djvCore)

