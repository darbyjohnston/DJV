:: In Windows Inkscape is shipped with a Python interpreter, you don't need to install it.
:: 
:: save both files in same directory as inkscape.exe and call
:: svg-objects-export.bat <svg-objects-export options> infiles+
@"./python/python.exe" svg-objects-export.py %* -i .\inkscape.com