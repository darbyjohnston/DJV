# \todo Remove translations from the output file that are no longer in the input file
#
# References:
# - https://github.com/GoogleCloudPlatform/python-docs-samples/blob/master/translate/cloud-client/quickstart.py

import sys
import json
from google.cloud import translate

def getTranslation(translateClient, text, language):
    translation = translateClient.translate(text, target_language=language)
    print text, "=", translation['translatedText']
    return translation['translatedText']

def run():
    
    inFile = sys.argv[1]
    outFile = sys.argv[2]
    language = sys.argv[3]
    
    inData = json.load(open(inFile))
    outData = []
    try:
        outData = json.load(open(outFile))
    except:
        pass

    translateClient = translate.Client()
    for inItem in inData:
        i = None
        for outItem in outData:
            if outItem['id'] == inItem['id']:
                i = outItem
                break
        if None == i:
            i = dict(inItem)
            itemLanguage = language
            if 'language' in inItem:
                itemLanguage = i['language']
            i['text'] = getTranslation(translateClient, inItem['text'], itemLanguage)
            i['source'] = inItem['text']
            outData.append(i)
        elif i['source'] != inItem['text']:
            itemLanguage = language
            if 'language' in inItem:
                itemLanguage = i['language']
            i['text'] = getTranslation(translateClient, inItem['text'], itemLanguage)
            i['source'] = inItem['text']

    with open(outFile, 'w') as f:
        json.dump(outData, f, indent = 4)
    
if __name__ == '__main__':
    run()

