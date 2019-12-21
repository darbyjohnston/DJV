#!/bin/sh

dirList=`cd $1 && find lib tests -mindepth 1 -maxdepth 1 -type d`
#echo "dirList="$dirList
for dir in $dirList
do
    gcnoList=`find $dir -name "*.gcno" -and \( -not -name "CMakeC*CompilerId.gcno" \)`
    #echo "gcnoList="$gcnoList
    for gcno in $gcnoList
    do
        #echo "gcno="$gcno
        srcName=`basename -s .gcno $gcno`
        #echo "srcName="$srcName
        src=`find $1/$dir -name $srcName`
        #echo "src="$src
        gcov -o $gcno $src
    done
done

