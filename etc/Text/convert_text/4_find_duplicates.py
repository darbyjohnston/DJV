import json
import os.path
from glob import glob
from html import unescape
import re
from pprint import pprint


djv_pattern = re.compile('DJV_TEXT\("(.*?)"\)')

src_files = glob("../../*/*/*.cpp") + glob("../../*/*/*.h")

source_strings = {}
duplicates = {}

for f_path in sorted(src_files):
    print(f_path)

    with open(f_path) as f:
        data = f.read()

    matches = djv_pattern.findall(data)
    for m in matches:
        if len(m):
            # print(m)
            if m in source_strings:
                source_strings[m].append(f_path)
            else:
                source_strings[m] = [f_path]

duplicates = {s: fp for s, fp in source_strings.items() if len(fp) > 1}

with open("duplicates", "w") as f:
    json.dump(duplicates, f, indent=2)
