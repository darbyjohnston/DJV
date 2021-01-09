# Find the ISPC compiler.
#
# This module defines the following variables:
#
# * ISPC_FOUND
# * ISPC_EXECUTABLE

find_program(ISPC_EXECUTABLE NAMES ispc)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    ISPC
    REQUIRED_VARS ISPC_EXECUTABLE)
