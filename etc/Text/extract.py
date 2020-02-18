import glob
import json
import re
import sys
from collections import OrderedDict 

def run():

    inDir = sys.argv[1]
    outFile = sys.argv[2]

    # Extract the strings from the source files.
    strings = []
    sourceFiles = glob.glob(inDir + "/*.cpp")
    sourceFiles.extend(glob.glob(inDir + "/*.h"))
    for sourceFile in sourceFiles:
        with open(sourceFile) as f:
            for line in f:
                match = re.findall('DJV_TEXT\("(.*?)"\)', line)
                for m in match:
                    if m not in strings:
                        strings.append(m)

    if len(strings):

        # Read the existing .text file.
        try:
            with open(outFile, 'r') as f:
                existing = json.load(f, object_pairs_hook=OrderedDict)
        except IOError:
            existing = []

        # Create the new .text file.
        data = {}
        for string in strings:
            found = False
            for i in existing:
                if i == string:
                    found = True
                    data[i] = existing[i]
                    break
            if not found:
                data[string] = string
        
        # Sort the data.
        sortedKeys = []
        for i in data:
            sortedKeys.append(i)
        sortedKeys = sorted(sortedKeys)
        sortedData = OrderedDict()
        for i in sortedKeys:
            sortedData[i] = data[i]
        
        with open(outFile, 'w') as f:
            json.dump(sortedData, f, indent = 4, ensure_ascii=False)
        
if __name__ == '__main__':
    run()

