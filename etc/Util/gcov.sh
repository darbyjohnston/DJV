#!/bin/sh

for gcno in `find $PWD -name "*.gcno"`
do
    #echo $gcno
    srcName=`basename -s .gcno $gcno`
    #echo $srcName
    src=`find $1 -name $srcName`
    #echo $src
    gcov -o $gcno $src
done

