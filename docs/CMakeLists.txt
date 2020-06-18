file(GLOB legal ${CMAKE_SOURCE_DIR}/LICENSE_*.txt)
file(COPY ${legal} DESTINATION ${DJV_BUILD_DIR}/docs)
file(GLOB html ${CMAKE_SOURCE_DIR}/docs/_site/*.html)
file(COPY ${html} DESTINATION ${DJV_BUILD_DIR}/docs)
file(COPY ${CMAKE_SOURCE_DIR}/docs/_site/assets DESTINATION ${DJV_BUILD_DIR}/docs)

install(
    FILES
    ${legal}
    DESTINATION ${DJV_INSTALL_DOCS})
install(
    FILES
    ${html}
    DESTINATION ${DJV_INSTALL_DOCS})
install(
    DIRECTORY
    _site/assets
    DESTINATION ${DJV_INSTALL_DOCS})
#install(
#    DIRECTORY
#    doxygen/html
#    DESTINATION ${DJV_INSTALL_DOCS}/doxygen)
