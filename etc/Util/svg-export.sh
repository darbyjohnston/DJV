#!/bin/sh

dpiList="48 96 144 192"

for svg in $1/*.svg
do
    for dpi in ${dpiList}
    do
        svg-objects-export-master/svg-objects-export.py -d $1 -f -P "" -PF "${dpi}DPI" --extra "--export-dpi $dpi" $svg
    done
done

