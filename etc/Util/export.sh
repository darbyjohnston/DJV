#!/bin/sh

dpiList="90 180"

for svg in *.svg
do
    for dpi in ${dpiList}
    do
        ../../third-party/svg-objects-export-master/svg-objects-export.py -f -P "FILE" -PF "${dpi}DPI" --extra "--export-dpi $dpi" $svg
    done
done

