#!/bin/sh

gcnoList=`find $PWD -name "*.gcno" -and \( -not -name "CMakeC*CompilerId.gcno" \)`
echo "gcnoList="$gcnoList

for gcno in $gcnoList
do
    #echo $gcno
    srcName=`basename -s .gcno $gcno`
    #echo $srcName
    src=`find $1 -name $srcName`
    #echo $src
    gcov -o $gcno $src
done
