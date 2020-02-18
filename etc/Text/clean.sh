#!/bin/sh

for f in "djvCore" "djvAV" "djvUI" "djvUIComponents" "djvDesktop" "djvViewApp" "djv_convert" "djv_test_pattern"
do
    for language in "cs" "da" "de" "el" "es" "fr" "it" "is" "ko" "ja" "pl" "pt" "ru" "sv" "zh"
    do
        rm -f $f.$language.text
    done
done

