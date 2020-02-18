import json
import os.path
from glob import glob
from html import unescape
import re


# Substitution from https://stackoverflow.com/a/59072515/4561348

# Get all new IDs
id_dict = {}
for id_file in glob("new_IDs/*"):
    with open(id_file) as f:
        id_dict.update(json.load(f))

# Create re pattern to match all ID in the source
djv_pattern = 'DJV_TEXT("{}")'
rep = dict((re.escape(djv_pattern.format(k)), djv_pattern.format(v)) for k, v in id_dict.items())
pattern = re.compile("|".join(rep.keys()))

#in_string = '''DJV_TEXT("Nothing to convert")    -writeSeq'''
#new_string = pattern.sub(lambda m: rep[re.escape(m.group(0))], in_string)
#print("new:", new_string)

src_files = glob("../../*/*/*.cpp") + glob("../../*/*/*.h")

for f_path in sorted(src_files):
    print(f_path)

    # Read in
    with open(f_path) as f:
        data = f.read()

    # Sub
    new_data = pattern.sub(lambda m: rep[re.escape(m.group(0))], data)

    # Write IN PLACE /!\ git checkout -- "../../**/*.cpp" "../../**/*.h"
    with open(f_path, "w") as f:
        f.write(new_data)
