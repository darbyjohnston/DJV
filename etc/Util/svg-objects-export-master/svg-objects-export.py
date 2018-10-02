#!/usr/bin/env python
"""
svg-objects-export

@link https://github.com/berteh/svg-objects-export

Export multiple SVG elements to other formats (png, pdf, ps, eps, svg), selecting them 
based on their ID with regular expressions, or XPath expression.

Useful for designing multiple icons in single file, sprite sheets, or multi-page 
documents with Inkscape (or another SVG editor). Easily generate low-resolution 
and high-resolution renders of some of the objects included in various SVG files
... and more. 

This script requires Inkscape (tested with 0.48)

 * This software is release under the terms of FRESH-JUICE-WARE LICENSE
 *
 * Berteh <https://gist.github.com/berteh/> wrote this file. You can do whatever 
 * you want with this stuff. If we meet some day, and you think this stuff is worth 
 * it, you can offer me a nice fresh juice.
 *
 * The author of this work hereby waives all claim of copyright (economic and moral)
 * in this work and immediately places it in the public domain; it may be used, 
 * distorted or destroyed in any manner whatsoever without further attribution or
 * notice to the creator. Constructive feedback is always welcome nevertheless.
"""
import argparse, sys, os, subprocess, cmd
import re, lxml


#constants (feel free to change these to your favorite defaults)
default_pattern = '^(rect|layer|path|use|g\d|svg|text|tspan|outline|image|circle|ellipse)\d'
if (sys.platform == 'win32'): inkscape_prog = 'C:\Progra~1\Inkscape\inkscape.com'
else: inkscape_prog = 'inkscape'
xpath_namespaces = {'svg':"http://www.w3.org/2000/svg",	\
	'sodipodi':"http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd", \
	'inkscape':"http://www.inkscape.org/namespaces/inkscape", \
	'xlink':"http://www.w3.org/1999/xlink", \
	're':"http://exslt.org/regular-expressions"}

#usually not code change below this line is necessary. any improvement suggestions is welcome.
#parse options
parser = argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter,
	description='''Exports multiple objects from an SVG file to various formats
(PNG, SVG, PS, EPS, PDF).''', 	
	usage="%(prog)s [-h] [-p PATTERN] [options] infiles+",
	epilog='''requirements
	This program requires Inkscape 0.48+ and Python 2.7+

default behaviour:
	The program exports by default all objects with an ID that has not
	been generated automatically by Inkscape.

	If you provide a custom pattern (-p) or xpath expression (-x), then exclude 
	(-e) is by default turned off, that is: your custom pattern or expression is
	used to define wich objects	are *included* unless you specify -e.

examples:

  %(prog)s --pattern '^export' in.svg
 	exports all objects with an ID starting with 'export' from in.svg
 	to PNG files in the current directory.

  %(prog)s --exclude --xpath '//svg:g | //svg:rect' in.svg
  	exports all objects that are no SVG group or rectangle, from in.svg to 
  	PNG files in current working directory. Namespaces available are: svg, 
  	inkscape, sodipodi, xlink, re (for regular expressions). 
  	See http://lxml.de for more on xpath in this program.

  %(prog)s --pattern '^(obj1|obj4)$' --prefix 'FILE_' in1.svg in2.svg
 	exports objects with IDs 'obj1' and 'obj4', from both in1 and in2 
 	files, to PNG files named in1_obj1.png, in1_obj4.png, in2_obj1.png 
 	and	in2_obj4.png.

  %(prog)s --silent --force --type eps --destdir vector/  ~/*.svg ~/tmp/*.svg
	exports all objects with an ID that does not resemble Inkscape
	default IDs, from any SVG file in user's home and tmp directories,
	to ./vector/ directory as EPS files, with no information displayed and
	overwritting existing files

  %(prog)s --exclude --pattern '[0-9]' --extra '--export-dpi 900' in.svg
	exports all objects with an ID containing no digit, from in.svg file,
	as PNG images with a resolution for	rasterization of 900 dpi. As 
	Inkscape uses 90 by default, this results in 10-times bigger images.


Additional examples: https://github.com/berteh/svg-objects-export/wiki



''')
parser.add_argument('infiles', nargs='+', 
	help='SVG file(s) to export objects from, wildcards are supported')
parser.add_argument('-p', '--pattern', default=default_pattern, 
	help='pattern (regular expression) to identify which objects to export or exclude from export (depending on --exclude). Default pattern matches most ID generated automatically by Inkscape (in exclude mode).')	
parser.add_argument('-x', '--xpath', default='', metavar='XPath EXPRESSION',
	help='XPath expression to identify which objects to export or exclude from export (depending on --exclude). This option is not considered if a pattern is provided (see --patern).')	
parser.add_argument('-e','--exclude', action='store_true', default=0,
	help='use pattern or expression to determine which objects to exclude from export, rather than include')
parser.add_argument ('-d', '--destdir', default='.',
	help='directory where images are exported to. Trailing slash is needed (backslash for windows), default is working directory.')
parser.add_argument('-s','--silent', action='store_true', default=False,
	help='do not print information to command line. Silent mode does not overwrite existing files by default, combine with --force if needed.')
parser.add_argument('-f','--force', action='store_true', default=False,
	help='overwrite existing files. Default is to warn and prompt user unless --silent is active.')
parser.add_argument('-P','--prefix', default='FILE_',
	help='prefix the generated file names with given PREFIX. "FILE" in the prefix is replaced with the svg file name. Default is "FILE_".')
parser.add_argument('-PF','--postfix', default='',
	help='postfix the generated file names with given POSTFIX.')
parser.add_argument('-i', '--inkscape', default=inkscape_prog,#  metavar='path_to_inkscape',
	help='path to inkscape command line executable')
parser.add_argument('-t', '--type', default='png', choices=['png', 'ps', 'eps', 'pdf', 'plain-svg'],
	help='export type (and suffix). png by default. See Inkscape --help for supported formats.')
parser.add_argument('-X', '--extra', metavar='Inkscape_Export_Options', default=' ',
	help='Extra options passed through (litterally) to inkscape for export. See Inkscape --help for more.')
parser.add_argument('-D','--debug', action='store_true', default=False,
	help='Generates (very) verbose output.')



def message(*msg):
	""" Utility "print" function that handles verbosity of messages
	"""
	if (not args.silent  or args.debug):
		print ''.join(msg)
	return
def debug(*msg):
	""" Utility "print" function that handles verbosity of messages
	"""
	if (args.debug):
		print msg
	return
def ife(test, if_result, else_result):
	if(test):
		return if_result
	return else_result

def printif(*args):
	print ife(args)

def confirm(prompt=None, resp=False): # adapted from http://code.activestate.com/recipes/541096-prompt-the-user-for-confirmation/
    """prompts for yes or no response from the user. Returns True for yes and
    False for no.

    'resp' should be set to the default value assumed by the caller when
    user simply types ENTER.
    """
    
    if prompt is None:
        prompt = 'Confirm'

    if resp:
        prompt = '%s %s/%s: ' % (prompt, 'Y', 'n')
    else:
        prompt = '%s %s/%s: ' % (prompt, 'N', 'y')
        
    while True:
        ans = raw_input(prompt)
        if not ans:
            return resp
        if ans not in ['y', 'Y', 'n', 'N']:
            print 'please enter y or n.'
            continue
        if ans == 'y' or ans == 'Y':
            return True
        if ans == 'n' or ans == 'N':
            return False

def exportObject(obj,args,prefix,postfix,extension,infile):
	debug("exporting ", obj)
	destfile = ''.join([args.destdir, prefix, obj, postfix,'.', extension])
	export = args.force
	if not args.force:
		if (not os.path.exists(destfile)):
			export = True
		elif not args.silent: # silent does not overwrite, use -sf if needed.
			export = confirm(prompt='File %s already exists, do you want to overwrite it?' % (destfile))				
	if export:				
		message('  '+obj+' to '+destfile)
		command = args.inkscape+' -i "'+obj+'" --export-'+args.type+' "'+destfile+'" '+args.extra+' '+infile
		debug("runnning "+command)
		run(command, shell=True)

## handle arguments
args = parser.parse_args()
if (args.silent):
	run = subprocess.check_output #
else:
	run = subprocess.check_call
# verify inkscape path
try:
	run([args.inkscape, "-V"])	
except Exception:
	print  '''Could not find inkscape command line executable, set --inkscape option accordingly.
It is usually /usr/bin/inkscape in linux, C:\Progra~1\Inkscape\inkscape.com in windows, and /Applications/Inkscape.app/Contents/Resources/bin/inkscape in Mac.'''
	sys.exit(2);
# set 'include' mode by default for custom pattern or xpath
if (args.exclude == 0):
	args.exclude = (args.pattern is default_pattern) and (args.xpath is '')
# fix 'plain-svg' extension
if (args.type == 'plain-svg'): extension = '.plain-svg.svg'
else: extension = args.type
# create destdir if needed
if not os.path.exists(args.destdir):
	message('creating directory: '+args.destdir)
	os.makedirs(args.destdir)
elif args.destdir is '.':
	args.destdir = '' #remove dot for current directory to ease the definition of destfile
#check for xpath
xpath_mode = (args.pattern is default_pattern) and not (args.xpath is '')
regexp_mode = not xpath_mode
#done with arguments processing
debug("arguments: ", args)



## process files
prefix = args.prefix
postfix = args.postfix

for infile in args.infiles:
	if ('FILE' in args.prefix): #update prefix if needed		
		prefix = args.prefix.replace('FILE',os.path.splitext(os.path.split(infile)[1])[0])
		#debug("  updated prefix to ", prefix, "  - infile is ", infile)	
	if (regexp_mode):
		#import re
		message("exporting from "+infile+" all objects "+ife(args.exclude,'not ','')+"matching "+args.pattern)
		objects_all = subprocess.check_output([args.inkscape, "--query-all", infile])	
		#message(objects)
		for obj in objects_all.splitlines():
			obj = obj.split(',')[0] #keep only ID:
			match = re.search(args.pattern, obj)
			#debug("object ",obj,ife(match, " matches"," does not match"))
			if ((args.exclude and (match == None)) or (not args.exclude and (match != None)) ):
				exportObject(obj,args,prefix,postfix,extension,infile)				
	elif (xpath_mode):
		from lxml import etree
		message("exporting from "+infile+" all objects "+ife(args.exclude,'not ','')+"matching "+args.xpath)
		parser = etree.XMLParser() #ns_clean=True)
		intree = etree.parse(infile, parser)
		if (len(parser.error_log) > 0):
			message ("Could not parse ",infile,":")
			debug(error_log)		
		find = etree.XPath("("+args.xpath+")/@id", namespaces=xpath_namespaces) #find the ids, not the objects
		objects = find(intree)
		message("found %i objects matching XPath" % len(objects))
		if (not args.exclude): #include mode
			for obj in objects:
				exportObject(obj,args,prefix,postfix,extension,infile)
		else: #exclude mode
			objects_all = subprocess.check_output([args.inkscape, "--query-all", infile])	
			#message(objects)
			for obj in objects_all.splitlines():
				obj = obj.split(',')[0] #keep only ID:
				if not(obj in objects):
					exportObject(obj,args,prefix,postfix,extension,infile)
		
