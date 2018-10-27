import sys
import xml.etree.ElementTree as ET
from google.cloud import translate

def run():
    inFile = sys.argv[1]
    outFile = sys.argv[2]
    language = sys.argv[3][:2]
    
    translateClient = translate.Client()
    tree = ET.parse(inFile)
    root = tree.getroot()
    for child in root:
        if "context" == child.tag:
            for child2 in child:
                if "message" == child2.tag:
                    text = ""
                    for child3 in child2:
                        if "source" == child3.tag:
                            text = child3.text
                    if len(text) > 0:
                        try:
                            translation = translateClient.translate(text, target_language=language)
                            print text, "=", translation['translatedText']
                            for child3 in child2:
                                if "translation" == child3.tag:
                                    child3.text = translation['translatedText']
                        except:
                            print "Error translating: ", text

    tree.write(outFile)
    
if __name__ == '__main__':
    run()

