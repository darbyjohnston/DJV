set(header
    Assert.h
    Box.h
    BoxInline.h
    Box2Inline.h
    Box3Inline.h
    BoxUtil.h
    BoxUtilInline.h
    CoreContext.h
    Core.h
    Debug.h
    DebugInline.h
    DebugLog.h
    Error.h
    ErrorUtil.h
    FileInfo.h
    FileInfoInline.h
    FileInfoUtil.h
    FileIO.h
    FileIOInline.h
    FileIOUtil.h
    ListUtil.h
    ListUtilInline.h
    Math.h
    MathInline.h
    Matrix.h
    MatrixInline.h
    Memory.h
    MemoryInline.h
    Plugin.h
    Range.h
    RangeInline.h
    RangeUtil.h
    RangeUtilInline.h
    Sequence.h
    SequenceInline.h
    SequenceUtil.h
    SequenceUtilInline.h
    SignalBlocker.h
    Speed.h
    StringUtil.h
    StringUtilInline.h
    System.h
    Time.h
    Timer.h
    User.h
    Util.h
    Vector.h
    VectorInline.h
    VectorUtil.h
    VectorUtilInline.h)
set(mocHeader
    CoreContext.h
    DebugLog.h
    FileInfo.h
    FileInfoUtil.h
    FileIO.h
    Memory.h
    Plugin.h
    Sequence.h
    Speed.h
    Time.h)
set(source
    Assert.cpp
    DebugLog.cpp
    CoreContext.cpp
    Debug.cpp
    Error.cpp
    ErrorUtil.cpp
    FileInfo.cpp
    FileInfoUtil.cpp
    FileIO.cpp
    FileIOUtil.cpp
    Math.cpp
    Memory.cpp
    Plugin.cpp
    Range.cpp
    Sequence.cpp
    SequenceUtil.cpp
    SignalBlocker.cpp
    Speed.cpp
    StringUtil.cpp
    System.cpp
    Time.cpp
    Timer.cpp
    User.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})
QT5_ADD_RESOURCES(rccSource djvCore.qrc)
QT5_CREATE_TRANSLATION(qmSource ${source}
    djvCore_en_US.ts
    djvCore_cs_CZ.ts)

add_library(djvCore ${header} ${source} ${mocSource} ${rccSource} ${qmSource})
target_link_libraries(djvCore
    ${djvThirdPartyOptional}
    Qt5
    GLM
    ${CMAKE_DL_LIBS})
set_target_properties(djvCore PROPERTIES FOLDER lib CXX_STANDARD 11)
if (djvInstallRpath)
    set_target_properties(
        djvCore PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")
endif (djvInstallRpath)

install(
    FILES ${header}
    DESTINATION include)
install(
    FILES ${qmSource}
    DESTINATION translations)
install(
    TARGETS djvCore
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

