#! python

import glob
import os
import sys

f = open(sys.argv[1], 'w')
f.write('[Files]\n')

# Appx manifest
f.write('"DJV\etc\Windows\AppxManifest.xml" "AppxManifest.xml"\n')

# Executable
f.write('"DJV-package\\build\\bin\\Release\\djv.exe" "bin\\djv.exe"\n')

# DLLs
for file in glob.glob('DJV-install-package\\bin\\*.dll'):
    f.write('"'+file+'" "bin\\'+os.path.basename(file)+'"\n')

# App icons
for file in glob.glob('DJV\\etc\\Windows\\*.png'):
    f.write('"'+file+'" "etc\\Icons\\'+os.path.basename(file)+'"\n')

# README and licenses
f.write('"DJV\\README.md" "README.md"\n')
for file in glob.glob('DJV\\LICENSE*'):
    f.write('"'+file+'" "'+os.path.basename(file)+'"\n')

# Text files
for file in glob.glob('DJV\\etc\\Text\\*.text'):
    f.write('"'+file+'" "etc\\Text\\'+os.path.basename(file)+'"\n')

# Icons
for dpi in glob.glob('DJV\\etc\\Icons\\*DPI'):
    for file in glob.glob(dpi+'\\*.png'):
        f.write('"'+file+'" "etc\\Icons\\'+os.path.basename(dpi)+'\\'+os.path.basename(file)+'"\n')

# Shaders
for file in glob.glob('DJV\\etc\\Shaders\\GL4_1\\*.glsl'):
    f.write('"'+file+'" "etc\\Shaders\\'+os.path.basename(file)+'"\n')

# Documentation
for file in glob.glob('DJV\\docs\\_site\\*.html'):
    f.write('"'+file+'" "docs\\'+os.path.basename(file)+'"\n')
for file in glob.glob('DJV\\docs\\_site\\assets\\*.png'):
    f.write('"'+file+'" "docs\\assets\\'+os.path.basename(file)+'"\n')
for file in glob.glob('DJV\\docs\\_site\\assets\\*.svg'):
    f.write('"'+file+'" "docs\\assets\\'+os.path.basename(file)+'"\n')
for file in glob.glob('DJV\\docs\\_site\\assets\\*.css'):
    f.write('"'+file+'" "docs\\assets\\'+os.path.basename(file)+'"\n')
