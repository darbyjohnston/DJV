import json
import os.path
from glob import glob
from html import unescape


for language in ("cs", "da", "de", "el", "es", "fr", "it", "is", "ko", "ja", "pl", "pt", "ru", "sv", "zh"):
    for new_id_path in glob("new_IDs/*.*.text"):
        # Open IDs map
        with open(new_id_path) as rf:
            new_ids = json.load(rf)

        # Open old language string files
        old_strings_path = new_id_path.replace("new", "old").replace(".en.", ".%s." % language)
        with open(old_strings_path) as rf:
            old_strings = json.load(rf)
            old_strings = {i["id"]: i["text"] for i in old_strings}

        # Generate new strings
        new_strings = {}
        for old_id, new_id in new_ids.items():
            if old_id in old_strings:
                # Get the translated text if it exists
                new_strings[new_id] = old_strings[old_id]
            else:
                # Default to English
                new_strings[new_id] = old_id

        # Open new language string files
        new_strings_path = os.path.basename(old_strings_path)
        with open(new_strings_path, "w") as wf:
            json.dump(new_strings, wf, ensure_ascii=False, indent=4)
