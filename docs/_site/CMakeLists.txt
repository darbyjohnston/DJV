file(GLOB legal ${CMAKE_SOURCE_DIR}/LICENSE_*.txt)
file(COPY ${legal} DESTINATION ${DJV_BUILD_DIR}/docs)
file(COPY _site DESTINATION ${DJV_BUILD_DIR}/docs/_site)

install(
    DIRECTORY
    _site
    DESTINATION ${DJV_INSTALL_DOCS}/_site)
#install(
#    DIRECTORY
#    doxygen/html
#    DESTINATION ${DJV_INSTALL_DOCS}/doxygen)
install(
    FILES
    ${legal}
    DESTINATION ${DJV_INSTALL_DOCS})

