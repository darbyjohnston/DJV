set(html
    Build.html
    ChangeList.html
    CommandLine.html
    Credits.html
    djv_convert.html
    djv_info.html
    djv_ls.html
    djv_view.html
    Documentation.html
    Download.html
    FAQ.html
    ImageFormats.html
    index.html
    Install.html
    Legal.html
    Screenshots.html
    test.html
    Testimonials.html)
set(images
    "images/Berkeley 2010-12-19.png"
    "images/Berkeley 2010-12-19 Tinted.png"
    "images/Bodega Bay 2013-01-02.png"
    "images/djv_view Color Picker Tool.png"
    "images/djv_view Histogram Tool.png"
    "images/djv_view Info Tool.png"
    "images/djv_view Magnify Tool.png"
    "images/djv_view.png"
    "images/Download.png"
    "images/index.png"
    "images/logo-filmreel.png"
    "images/Miscellaneous Preferences.png"
    "images/Screenshot 1.png"
    "images/Screenshot 2.png"
    "images/Screenshot 3.png")
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

