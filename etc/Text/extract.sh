#!/bin/sh

for dir in ../../lib/*/ ../../bin/*/ ../../examples/*/
do
    python ./extract.py $dir $(basename $dir).en.text
done

