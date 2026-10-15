# Read by CPack when it runs (CPACK_PROJECT_CONFIG_FILE), not when the build
# is configured: the signing command only exists once CI has signed the
# programs, which is after the build was configured.
#
# The Windows installer writes its uninstaller onto the user's machine when it
# installs, so the uninstaller was never in reach of the signing steps and
# Windows called its publisher unknown. NSIS can instead make the uninstaller
# when the installer is compiled, hand it to a command to sign, and carry the
# signed copy (!uninstfinalize, NSIS 3.08). The command comes from CI in
# DJV_NSIS_SIGN_COMMAND, with "%1" standing for the file; without it the uninstaller is left
# unsigned, as for any build made away from CI.
if(CPACK_GENERATOR STREQUAL "NSIS" AND NOT "$ENV{DJV_NSIS_SIGN_COMMAND}" STREQUAL "")
    set(CPACK_NSIS_DEFINES "!uninstfinalize '$ENV{DJV_NSIS_SIGN_COMMAND} \"%1\"' = 0")
endif()
