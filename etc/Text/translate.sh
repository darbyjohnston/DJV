#!/bin/sh

for file in *.en.text
do
    if [[ -f $file ]]; then
        echo $file
        #for language in "fr"
        for language in "cs" "da" "de" "el" "es" "fr" "ko" "ja" "pl" "ru" "sv" "zh"
        do
            outfile=`echo $file | sed -e "s/\.en\./\.$language\./"`
            echo "    "$outfile
            python ./translate.py $file $outfile $language
        done
    fi
done

