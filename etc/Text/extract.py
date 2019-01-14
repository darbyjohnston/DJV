import glob
import json
import re
import sys

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
                match = re.findall('DJV_TEXT\("(.*)"\s*,\s*"(.*)"\)', line)
                strings.extend(match)
    
    if len(strings):
    
        # Read the .text file.
        try:
            with open(outFile, 'r') as f:
                data = json.load(f)
        except IOError:
            data = []
        
        # Add new strings to the .text file.
        for string in strings:
            i = None
            for item in data:
                if item['id'] == string[1]:
                    i = item
                    break
            if None == i:
                data.append({'nameSpace' : string[0], 'id' : string[1], 'text': string[1], 'description': ''})

        # Write the .text file.
        with open(outFile, 'w') as f:
            json.dump(data, f, indent = 4)
    
if __name__ == '__main__':
    run()

