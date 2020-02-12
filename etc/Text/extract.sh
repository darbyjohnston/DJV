#!/bin/sh

for dir in ../../lib/*/ ../../bin/*/ ../../examples/*/
do
    python3 ./extract.py $dir $(basename $dir).en.text
done

