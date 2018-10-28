#!/bin/sh

appname=`basename $0 | sed s,\.sh$,,`
#echo "appname="$appname

dirname=`dirname $0`
#echo "dirname="$dirname

root=`dirname $dirname`
#echo "root="$root

export LD_LIBRARY_PATH=$root"/lib"

$dirname/$appname "$@"

