# References:
# - https://github.com/GoogleCloudPlatform/python-docs-samples/blob/master/translate/cloud-client/quickstart.py

import sys
import json
from google.cloud import translate

def run():
    
    inFile = sys.argv[1]
    outFile = sys.argv[2]
    language = sys.argv[3]
    
    data = json.load(open(inFile))

    translateClient = translate.Client()
    for item in data:
        itemLanguage = language
        if 'language' in item:
            itemLanguage = item['language']
        translation = translateClient.translate(item['text'], target_language=itemLanguage)
        print item['text'], "=", translation['translatedText']
        item['text'] = translation['translatedText']

    with open(outFile, 'w') as f:
        json.dump(data, f, indent = 4)
    
if __name__ == '__main__':
    run()

