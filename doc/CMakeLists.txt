set(html
    Build.html
    BuildLinux.html
    BuildWindows.html
    CommandLine.html
    Credits.html
    Documentation.html
    Download.html
    FAQ.html
    FileBrowser.html
    FileSequences.html
    ImageFileFormats.html
    Legal.html
    Screenshots.html
    Style.css
    Testimonials.html
    ViewCommandLine.html
    ViewFiles.html
    ViewImageView.html
    ViewImages.html
    ViewPlayback.html
    ViewTools.html
    ViewWindows.html
    djv_convert.html
    djv_info.html
    djv_ls.html
    djv_view.html
    index.html)
set(images
    "images/Berkeley 2010-12-19 Tinted.jpg"
    "images/Bodega Bay 2013-01-02 Tinted.jpg"
    "images/ColorPickerIcon96DPI.png"
    "images/DownloadIcon.png"
    "images/FileBrowser.png"
    "images/FileCloseIcon96DPI.png"
    "images/FileIcon96DPI.png"
    "images/FileOpenIcon96DPI.png"
    "images/FileReloadIcon96DPI.png"
    "images/FrameEndIcon96DPI.png"
    "images/FrameNextIcon96DPI.png"
    "images/FramePrevIcon96DPI.png"
    "images/FrameStartIcon96DPI.png"
    "images/HistogramIcon96DPI.png"
    "images/InOutPointsIcon96DPI.png"
    "images/InPointMarkIcon96DPI.png"
    "images/InPointResetIcon96DPI.png"
    "images/InfoIcon96DPI.png"
    "images/MagnifyIcon96DPI.png"
    "images/OutPointMarkIcon96DPI.png"
    "images/OutPointResetIcon96DPI.png"
    "images/PlayForwardIcon96DPI.png"
    "images/PlayLoopOnceIcon96DPI.png"
    "images/PlayLoopPingPongIcon96DPI.png"
    "images/PlayLoopRepeatIcon96DPI.png"
    "images/PlayReverseIcon96DPI.png"
    "images/PlayStopIcon96DPI.png"
    "images/Screenshot 1.png"
    "images/Screenshot 2.png"
    "images/Screenshot 3.png"
    "images/ViewColorPickerTool.png"
    "images/ViewFitIcon96DPI.png"
    "images/ViewHistogramTool.png"
    "images/ViewInfoTool.png"
    "images/ViewMagnifyTool.png"
    "images/ViewZoomInIcon96DPI.png"
    "images/ViewZoomOutIcon96DPI.png"
    "images/ViewZoomResetIcon96DPI.png"
    "images/WindowCloseIcon96DPI.png"
    "images/WindowDuplicateIcon96DPI.png"
    "images/WindowFitIcon96DPI.png"
    "images/WindowFullScreenIcon96DPI.png"
    "images/WindowNewIcon96DPI.png"
    "images/index.png"
    "images/logo-filmreel.png")
set(legal
    ../LICENSE_CMake.txt
    ../LICENSE_FFmpeg.txt
    ../LICENSE_GLM.txt
    ../LICENSE_libjpeg.txt
    ../LICENSE_libpng.txt
    ../LICENSE_libtiff.txt
    ../LICENSE_OpenAL.txt
    ../LICENSE_OpenEXR.txt
    ../LICENSE_Qt.txt
    ../LICENSE.txt
    ../LICENSE_zlib.txt)

file(COPY ${html} DESTINATION ${djvBuildDir}/doc)
file(COPY ${images} DESTINATION ${djvBuildDir}/doc/images)

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
