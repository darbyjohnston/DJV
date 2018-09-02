set(header
    djvAssert.h
    djvBox.h
    djvBox2Inline.h
    djvBox3Inline.h
    djvBoxInline.h
    djvBoxUtil.h
    djvBoxUtilInline.h
    djvColor.h
    djvColorInline.h
    djvColorUtil.h
    djvColorUtilInline.h
    djvColorProfile.h
    djvCoreContext.h
    djvCoreExport.h
    djvCore.h
    djvDebug.h
    djvDebugInline.h
    djvDebugLog.h
    djvError.h
    djvErrorUtil.h
    djvFileInfo.h
    djvFileInfoInline.h
    djvFileInfoUtil.h
    djvFileIo.h
    djvFileIoInline.h
    djvFileIoUtil.h
    djvImage.h
    djvImageContext.h
    djvImageIo.h
    djvImageTags.h
    djvImageUtil.h
    djvListUtil.h
    djvListUtilInline.h
    djvMath.h
    djvMathInline.h
    djvMatrix.h
    djvMatrix3Inline.h
    djvMatrix4Inline.h
    djvMatrixInline.h
    djvMatrixUtil.h
    djvMatrixUtilInline.h
    djvMemory.h
    djvMemoryBuffer.h
    djvMemoryBufferInline.h
    djvMemoryInline.h
    djvOpenGl.h
    djvOpenGlImage.h
    djvOpenGlOffscreenBuffer.h
    djvOpenGlLut.h
    djvOpenGlShader.h
    djvOpenGlTexture.h
    djvPixel.h
    djvPixelData.h
    djvPixelDataInline.h
    djvPixelDataUtil.h
    djvPixelInline.h
    djvPlugin.h
    djvRange.h
    djvRangeInline.h
    djvRangeUtil.h
    djvRangeUtilInline.h
    djvSequence.h
    djvSequenceInline.h
    djvSequenceUtil.h
    djvSequenceUtilInline.h
    djvSignalBlocker.h
    djvSpeed.h
    djvStringUtil.h
    djvStringUtilInline.h
    djvSystem.h
    djvTime.h
    djvTimer.h
    djvUser.h
    djvUtil.h
    djvVector.h
    djvVector2Inline.h
    djvVector3Inline.h
    djvVectorInline.h
    djvVectorUtil.h
    djvVectorUtilInline.h)
set(mocHeader
    djvColorProfile.h
    djvCoreContext.h
    djvDebugLog.h
    djvFileInfo.h
    djvFileInfoUtil.h
    djvFileIo.h
    djvImageIo.h
    djvImageTags.h
    djvMemory.h
    djvOpenGlImage.h
    djvPixelData.h
    djvPixel.h
    djvPlugin.h
    djvSequence.h
    djvSpeed.h
    djvTime.h)
set(source
    djvAssert.cpp
    djvDebugLog.cpp
    djvColor.cpp
    djvColorUtil.cpp
    djvColorProfile.cpp
    djvCoreContext.cpp
    djvDebug.cpp
    djvError.cpp
    djvErrorUtil.cpp
    djvFileInfo.cpp
    djvFileInfoUtil.cpp
    djvFileIo.cpp
    djvFileIoUtil.cpp
    djvImage.cpp
    djvImageContext.cpp
    djvImageIo.cpp
    djvImageTags.cpp
    djvImageUtil.cpp
    djvMath.cpp
    djvMemory.cpp
    djvOpenGl.cpp
    djvOpenGlImage.cpp
    djvOpenGlImageDraw.cpp
    djvOpenGlOffscreenBuffer.cpp
    djvOpenGlLut.cpp
    djvOpenGlShader.cpp
    djvOpenGlTexture.cpp
    djvPixel.cpp
    djvPixelConvert.cpp
    djvPixelData.cpp
    djvPixelDataUtil.cpp
    djvPlugin.cpp
    djvSequence.cpp
    djvSequenceUtil.cpp
    djvSignalBlocker.cpp
    djvSpeed.cpp
    djvStringUtil.cpp
    djvSystem.cpp
    djvTime.cpp
    djvTimer.cpp
    djvUser.cpp
    djvVectorUtil.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})
QT5_ADD_RESOURCES(rccSource djvCore.qrc)
QT5_CREATE_TRANSLATION(qmSource ${source}
    djvCore_en_US.ts
    djvCore_cs_CZ.ts)

include_directories(${djvCoreIncludeDirs})
add_library(djvCore ${header} ${source} ${mocSource} ${rccSource} ${qmSource})
target_link_libraries(djvCore ${djvCoreDeps})

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

