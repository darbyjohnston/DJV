# \todo Remove translations from the output file that are no longer in the input file
#
# References:
# - https://github.com/GoogleCloudPlatform/python-docs-samples/blob/master/translate/cloud-client/quickstart.py

import sys
import json
from collections import OrderedDict 
from google.cloud import translate_v2

def getTranslation(translateClient, text, language):
    translation = translateClient.translate(text, target_language=language)
    print(text, "=", translation['translatedText'])
    return translation['translatedText']

def run():
    
    # Parse the command-line arguments.
    inFile = sys.argv[1]
    outFile = sys.argv[2]
    language = sys.argv[3]
    
    # Read the input and output files.
    inData = json.load(open(inFile), object_pairs_hook=OrderedDict)
    outData = []
    try:
        outData = json.load(open(outFile), object_pairs_hook=OrderedDict)
    except:
        pass

    # Create the translation client.
    translateClient = translate_v2.Client()
    
    for inItem in inData:

        # The language passed on the command-line is used by default,
        # but this may be over-ridden by the input file.
        itemLanguage = language
        if 'language' in inItem:
            itemLanguage = inItem['language']

        # Check if the item exists in the output file.
        i = None
        for outItem in outData:
            if outItem['id'] == inItem['id']:
                i = outItem
                break

        if None == i:

            # The item doesn't exist in the output file, add it.
            i = OrderedDict(inItem)
            if itemLanguage != 'en':
                i['text'] = getTranslation(translateClient, inItem['text'], itemLanguage)
            else:
                i['text'] = inItem['text']
            i['source'] = inItem['text']
            outData.append(i)
            
        else:

            # The item exists in the output file, only translate
            # it if the text or input language has changed.
            textChanged = i['source'] != inItem['text']
            languageChanged = False
            if 'language' in inItem:
                if 'language' in i:
                    languageChanged = inItem['language'] != i['language']
                else:
                    languageChanged = True
                i['language'] = itemLanguage
            if textChanged or languageChanged:
                if itemLanguage != 'en':
                    i['text'] = getTranslation(translateClient, inItem['text'], itemLanguage)
                else:
                    i['text'] = inItem['text']
                i['source'] = inItem['text']

    with open(outFile, 'w') as f:
        json.dump(outData, f, indent = 4, ensure_ascii=False)
    
if __name__ == '__main__':
    run()

