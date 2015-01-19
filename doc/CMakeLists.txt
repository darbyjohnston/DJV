configure_file(
    ${PROJECT_SOURCE_DIR}/doc/index.html.in
    ${PROJECT_BINARY_DIR}/doc/index.html
    @ONLY)
configure_file(
    ${PROJECT_SOURCE_DIR}/doc/djvInstall.html.in
    ${PROJECT_BINARY_DIR}/doc/djvInstall.html
    @ONLY)

set(html
    djvChangeList.html
    djvCommandLine.html
    djvCredits.html
    djvFaq.html
    djvHelp.html
    djvImageIo.html
    djvLegal.html
    djvScreenshots.html
    djvTestimonials.html
    djv_convert.html
    djv_info.html
    djv_ls.html
    djv_view.html
    djvStyle.css
    ${PROJECT_BINARY_DIR}/doc/index.html
    ${PROJECT_BINARY_DIR}/doc/djvInstall.html)

set(images
    images/djvColorPickerIcon.png
    images/djvDirPrevIcon.png
    images/djvDirReloadIcon.png
    images/djvDirUpIcon.png
    images/djvFileBrowser.png
    images/djvFileCloseIcon.png
    images/djvFileOpenIcon.png
    images/djvFileReloadIcon.png
    images/djvFrameEndIcon.png
    images/djvFrameNextIcon.png
    images/djvFramePrevIcon.png
    images/djvFrameStartIcon.png
    images/djvHistogramIcon.png
    images/djvInPointMarkIcon.png
    images/djvInPointResetIcon.png
    images/djvInfoIcon.png
    images/djvLockIcon.png
    images/djvLogo.gif
    images/djvMagnifyIcon.png
    images/djvOutPointMarkIcon.png
    images/djvOutPointResetIcon.png
    images/djvPlayForwardIcon.png
    images/djvPlayInOutIcon.png
    images/djvPlayReverseIcon.png
    images/djvPlayStopIcon.png
    images/djvTitle.gif
    images/djvView.png
    images/djvViewColorPickerTool.png
    images/djvViewColorPickerToolDiagram.png
    images/djvViewDiagram.png
    images/djvViewDisplayProfile.png
    images/djvViewFileMenu.png
    images/djvViewFitIcon.png
    images/djvViewHelpMenu.png
    images/djvViewHistogramTool.png
    images/djvViewHistogramToolDiagram.png
    images/djvViewImageMenu.png
    images/djvViewInfoTool.png
    images/djvViewInfoToolDiagram.png
    images/djvViewMagnifyTool.png
    images/djvViewMagnifyToolDiagram.png
    images/djvViewPlaybackDiagram.png
    images/djvViewPlaybackMenu.png
    images/djvViewToolMenu.png
    images/djvViewTools.png
    images/djvViewToolsSmall.png
    images/djvViewViewMenu.png
    images/djvViewWindowMenu.png
    images/djvViewZoomInIcon.png
    images/djvViewZoomOutIcon.png
    images/djvViewZoomResetIcon.png
    images/djvWindowCloseIcon.png
    images/djvWindowCopyIcon.png
    images/djvWindowFitIcon.png
    images/djvWindowFullScreenIcon.png
    images/djvWindowNewIcon.png
    images/index.png
    images/indexSmall.png
    images/OpenGL.png)

set(legal
    ../LICENSE
    ../LICENSE.CMake
    ../LICENSE.FFmpeg
    ../LICENSE.GLEW
    ../LICENSE.libjpeg
    ../LICENSE.libpng
    ../LICENSE.libquicktime
    ../LICENSE.libtiff
    ../LICENSE.OpenEXR
    ../LICENSE.Qt
    ../LICENSE.zlib)

install(
    FILES
    ${html}
    DESTINATION doc)

install(
    FILES
    ${images}
    DESTINATION doc/images)

#install(
#    DIRECTORY
#    doxygen/html
#    DESTINATION doc/doxygen)

install(
    FILES
    ${legal}
    DESTINATION doc)
