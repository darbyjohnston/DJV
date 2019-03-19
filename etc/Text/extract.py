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
                match = re.findall('DJV_TEXT\("(.*?)"\)', line)
                for m in match:
                    if m not in strings:
                        strings.append(m)
    
    if len(strings):
    
        # Read the existing .text file.
        try:
            with open(outFile, 'r') as f:
                existing = json.load(f)
        except IOError:
            existing = []
        
        # Create the new .text file.
        data = []
        for string in strings:
            found = False
            for item in existing:
                if item['id'] == string:
                    found = True
                    data.append(item)
                    break
            if not found:
                data.append({'id' : string, 'text': string, 'description': ''})
        with open(outFile, 'w') as f:
            json.dump(data, f, indent = 4)
    
if __name__ == '__main__':
    run()

