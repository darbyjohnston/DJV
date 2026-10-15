#!/bin/sh

# Build the dependencies and then DJV, into directories beside the current
# one. What to build is in etc/Config/*.cmake rather than here: this script is
# only the part that differs between platforms.

set -e
set -x

SOURCE_DIR=$1
BUILD_TYPE=$2
CONFIG=${3:-default}
CONFIG_FILE=$SOURCE_DIR/etc/Config/$CONFIG.cmake

# Build with every core unless told otherwise; "cmake --build" reads this.
if [ -z "$CMAKE_BUILD_PARALLEL_LEVEL" ]; then
    CMAKE_BUILD_PARALLEL_LEVEL=$(nproc)
    export CMAKE_BUILD_PARALLEL_LEVEL
fi

# Check out the submodules the first time, and never move them afterwards.
# A submodule you have work in is not this script's to reset: the build it
# would then make is not the source you have, and with changes in the way it
# stops on a git error about a checkout you did not ask for. Same rule and
# same test as CMakeLists.txt, which initialises only when there is nothing
# there to lose.
if [ ! -f $SOURCE_DIR/deps/tlRender/CMakeLists.txt ]; then
    git -C $SOURCE_DIR submodule update --init --recursive
fi

# SBUILD_STAGES picks the stages: "deps" builds only the dependencies, into
# install-<type>, and "project" only DJV, against dependencies already
# there. Unset, all of them. CI caches the dependencies' install directory
# between the two, which is all the project needs of them.
for STAGE in "deps/tlRender/deps/ftk/etc/SuperBuild ftk" "deps/tlRender/etc/SuperBuild tl" ". build"; do
    set -- $STAGE
    if [ "$2" = "build" ] && [ "$SBUILD_STAGES" = "deps" ]; then
        exit 0
    fi
    if [ "$2" != "build" ] && [ "$SBUILD_STAGES" = "project" ]; then
        continue
    fi
    cmake \
        -S $SOURCE_DIR/$1 \
        -B $2-$BUILD_TYPE \
        -C $CONFIG_FILE \
        -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
        -DCMAKE_INSTALL_PREFIX=$PWD/install-$BUILD_TYPE \
        -DCMAKE_PREFIX_PATH=$PWD/install-$BUILD_TYPE
    cmake --build $2-$BUILD_TYPE --config $BUILD_TYPE
done

# The install directory is how everything downstream finds what was built: the
# tests import the bindings from there, and packaging reads it. feather-tk and
# tlRender both end this way; without it the libraries stay in the build tree,
# where the Python tests cannot find them on Windows.
cmake --build build-$BUILD_TYPE --config $BUILD_TYPE --target install
