set(html
    ChangeList.html
    CommandLine.html
    Credits.html
    Documentation.html
    Download.html
    FAQ.html
    ImageIO.html
    Legal.html
    Screenshots.html
    Style.css
    Testimonials.html
    djv_convert.html
    djv_info.html
    djv_ls.html
    djv_view.html
    ${PROJECT_BINARY_DIR}/doc/index.html
    ${PROJECT_BINARY_DIR}/doc/Install.html)
set(images
    images/Berkeley 2010-12-19.png
    images/Bodega Bay 2013-01-02.png
    images/index.png
    images/logo-filmreel.png)
set(legal
    ../LICENSE.txt
    ../LICENSE_CMake.txt
    ../LICENSE_FFmpeg.txt
    ../LICENSE_GLM.txt
    ../LICENSE_OpenAL.txt
    ../LICENSE_OpenEXR.txt
    ../LICENSE_Qt.txt
    ../LICENSE_libjpeg.txt
    ../LICENSE_libpng.txt
    ../LICENSE_libtiff.txt
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

