svg-objects-export
==================

Export multiple SVG elements to other formats (png, pdf, ps, eps, svg), selecting them based on their ID with regular expressions, or XPath expression.

Useful for designing multiple icons in single file, sprite sheets, or multi-page documents with Inkscape (or other SVG editor). Easily generate low-resolution and high-resolution renders of some of the objects included in various SVG files... and more. 


usage
-----
svg-objects-export.py [-h] [-p PATTERN] [options] infiles


arguments
---------

**positional arguments:**

    infiles               SVG file(s) to export objects from, wildcards are
                          supported

**optional arguments:**

    -h, --help            show this help message and exit
    -p PATTERN, --pattern PATTERN
                          pattern (regular expression) to identify which objects
                          to export or exclude from export (depending on
                          --exclude). Default pattern matches most ID generated
                          automatically by Inkscape (in exclude mode).
    -x XPath EXPRESSION, --xpath XPath EXPRESSION
                          XPath expression to identify which objects to export 
                          or exclude from export (depending on --exclude). This
                          option is not considered if a pattern is provided (see
                          --patern).
    -e, --exclude         use pattern or expression to determine which objects 
                          to exclude from export, rather than include
    -d DESTDIR, --destdir DESTDIR
                          directory where images are exported to. Trailing slash
                          is needed (backslash for windows), default is working
                          directory.
    -s, --silent          do not print information to command line. Silent mode
                          does not overwrite existing files by default, combine
                          with --force if needed.
    -f, --force           overwrite existing files. Default is to warn and
                          prompt user unless --silent is active.
    -P PREFIX, --prefix PREFIX
                          prefix the generated file names with given PREFIX.
                          "FILE" in the prefix is replaced with the svg file
                          name.
    -i INKSCAPE, --inkscape INKSCAPE
                          path to inkscape command line executable
    -t {png,ps,eps,pdf,plain-svg}, --type {png,ps,eps,pdf,plain-svg}
                          export type (and suffix). png by default. See Inkscape
                          --help for supported formats (png, ps, eps, pdf,
                          plain-svg).
    -X Inkscape_Export_Options, --extra Inkscape_Export_Options
                          Extra options passed through (litterally) to inkscape
                          for export. See Inkscape --help for more.
    -D, --debug           Generates (very) verbose output.




requirements
------------
This program requires Inkscape 0.48+ and Python 2.7+

default behaviour
-----------------
The program exports by default all objects with an ID that has not
been generated automatically by Inkscape.

If you provide a custom pattern (-p) or xpath expression (-x), then exclude 
(-e) is by default turned off, that is: your custom pattern or expression is
used to define wich objects are *included* unless you specify -e.

examples
--------

    svg-objects-export.py --pattern '^export' in.svg

exports all objects with an ID starting with 'export' from in.svg
to PNG files in the current directory.


     svg-objects-export.py --exclude --xpath '//svg:g | //svg:rect' in.svg

exports all objects that are no SVG group or rectangle, from in.svg to 
PNG files in current working directory. Namespaces available are: svg, 
inkscape, sodipodi, xlink, re (for regular expressions). 
See http://lxml.de for more on xpath in this program.


    svg-objects-export.py --pattern '^(obj1|obj4)$' --prefix 'FILE_' in1.svg in2.svg

exports objects with IDs 'obj1' and 'obj4', from both in1 and in2 
files, to PNG files named in1_obj1.png, in1_obj4.png, in2_obj1.png 
and in2_obj4.png.


    svg-objects-export.py --silent --force --type eps --destdir vector/  ~/*.svg ~/tmp/*.svg

exports all objects with an ID that does not resemble Inkscape
default IDs, from any SVG file in user's home and tmp directories,
to vector/ directory as EPS files, with no information displayed and
overwritting existing files


    svg-objects-export.py --exclude --pattern '[0-9]' --extra '--export-dpi 900' in.svg

exports all objects with an ID containing no digit, from in.svg file,
as PNG images with a resolution for rasterization of 900 dpi. As 
Inkscape uses 90 by default, this results in 10-times bigger images.

Additional examples: https://github.com/berteh/svg-objects-export/wiki


license
-------
This software is release under the terms of FRESH-JUICE-WARE LICENSE:

[Berteh](https://github.com/berteh/) wrote this file. You can do whatever 
you want with this stuff. If we meet some day, and you think this stuff is worth 
it, you can offer me a nice fresh juice.

The author of this work hereby waives all claim of copyright (economic and moral)
in this work and immediately places it in the public domain; it may be used, 
distorted or destroyed in any manner whatsoever without further attribution or
notice to the creator. Constructive feedback is always welcome nevertheless.
