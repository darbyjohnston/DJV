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
    outData = OrderedDict()
    try:
        outData = json.load(open(outFile), object_pairs_hook=OrderedDict)
    except:
        pass

    # Create the translation client.
    translateClient = translate_v2.Client()
    
    # Only translate strings that not already in the output file.
    for key in inData:
        if not key in outData:
            outData[key] = getTranslation(translateClient, inData[key], language)

    # Sort the output data.
    keys = []
    for key in outData:
        keys.append(key)
    sortedKeys = sorted(keys)
    sortedData = OrderedDict()
    for key in sortedKeys:
        sortedData[key] = outData[key]

    # Write the output file.
    with open(outFile, 'w') as f:
        json.dump(sortedData, f, indent = 4, ensure_ascii=False)
    
if __name__ == '__main__':
    run()

