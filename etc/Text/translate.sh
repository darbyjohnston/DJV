#!/bin/sh

for file in *.en.text
do
    if [[ -f $file ]]; then
        echo $file
        #for language in "de"
        for language in "cs" "da" "de" "el" "es" "fr" "it" "is" "ko" "ja" "pl" "pt" "ru" "sv" "zh"
        do
            outfile=`echo $file | sed -e "s/\.en\./\.$language\./"`
            echo "    "$outfile
            python ./translate.py $file $outfile $language
        done
    fi
done

