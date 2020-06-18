#! python

import glob
import os
import sys

f = open(sys.argv[1], 'w')
f.write('[Files]\n')

# Appx manifest
f.write('"DJV\\etc\\Windows\\AppxManifest.xml" "AppxManifest.xml"\n')

# Executable
f.write('"DJV-package\\build\\bin\\Release\\djv.exe" "bin\\djv.exe"\n')

# DLLs
for file in glob.glob('DJV-install-package\\bin\\*.dll'):
    f.write('"'+file+'" "bin\\'+os.path.basename(file)+'"\n')

# App icons
for file in glob.glob('DJV\\etc\\Windows\\*.png'):
    f.write('"'+file+'" "etc\\Icons\\'+os.path.basename(file)+'"\n')

# etc
os.chdir("DJV-package\\build")
for file in glob.glob('etc\\**', recursive=True):
    if os.path.isfile(file):
        f.write('"DJV-package\\build\\'+file+'" "'+file+'"\n')

# docs
for file in glob.glob('docs\\**', recursive=True):
    if os.path.isfile(file):
        f.write('"DJV-package\\build\\'+file+'" "'+file+'"\n')
