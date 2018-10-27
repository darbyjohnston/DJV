#!/bin/sh

for file in `find ../../lib -name "*en_US.ts"`
do
    if [[ -f $file ]]; then
        echo "input: "$file
        for language in "ja_JP"
        #for language in "cs" "da" "de" "el" "es" "fr" "ko" "ja" "pl" "ru" "sv" "zh"
        do
            outfile=`echo $file | sed -e "s/en_US/$language/"`
            echo "output: "$outfile
            python ./translate.py $file $outfile $language
        done
    fi
done

