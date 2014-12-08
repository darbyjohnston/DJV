#!/bin/sh

appname=`basename $0 | sed s,\.sh$,,`

dirname=`dirname $0`
tmp="${dirname#?}"

if [ "${dirname%$tmp}" != "/" ]; then
    dirname=$PWD/$dirname
fi

export DYLD_FALLBACK_LIBRARY_PATH=$dirname"/../lib"

$dirname/$appname "$@"

