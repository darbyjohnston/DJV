import json
import sys
import webbrowser

def elide(value, max=30):
	if len(value) > max:
		value = value[0:max] + "..."
	return value

def isPathSeparator(value):
	return '/' == value or '\\' == value
def isExtensionSeparator(value):
	return '.' == value
def isNumber(value):
	return value in ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']
def splitFileName(value):
	out = [ str(), str(), str(), str() ]
	l = len(value)
	if l > 0:
		lastPathSeparator = -1
		lastDot = -1
		for i in range(l):
			if isPathSeparator(value[i]):
				lastPathSeparator = i
			elif isExtensionSeparator(value[i]):
				lastDot = i
		start = 0
		end = l - 1
		if lastPathSeparator != -1:
			start = lastPathSeparator + 1
			out[0] = value[0:start]
		if lastDot > 0 and lastDot > lastPathSeparator + 1:
			end = lastDot - 1
			out[3] = value[lastDot:l]
		if (isNumber(value[end])):
			i = end
			while (isNumber(value[end])):
				end = end - 1
			out[2] = value[end + 1:i + 1]
		out[1] = value[start:end + 1]
	return out

# Parse the command line arguments.
if len(sys.argv) < 3:
	print("Usage: AnnotateExportToHTML.py (JSON file) (image sequence)")
	exit(1)
inputJSON = sys.argv[1]
imageSeq = sys.argv[2]

# Read the input JSON file.
data = {}
try:
	with open(inputJSON, "r") as f:
		data = json.load(f)
except IOError:
	print("Error reading \"" + inputJSON + "\"")
	exit(1)

# Write the output HTML file.
split = splitFileName(inputJSON)
split[3] = ".html"
outputHTML = split[0] + split[1] + split[2] + split[3]
try:
	with open(outputHTML, "w") as f:
	
		# Write the header.
		f.write("<html>\n")
		f.write("<head>\n")
		f.write("<style>\n")
		f.write("    .body { margin:0; color:#ffffff; background-color:#223344; font-family:sans-serif; }\n")
		f.write("    .title { margin-bottom:20; font-size:48; }\n")
		f.write("    .header { padding:10; padding-left:20; margin-bottom:20; font-size:24; background-color:#112233; }\n")
		f.write("    .text { font-size:16; }\n")
		f.write("    .textSmall { font-size:12; }\n")
		f.write("    .section { margin-bottom:20; }\n")
		f.write("    .sectionTitle { margin:20; }\n")
		f.write("    .sectionSub { margin:20; }\n")
		f.write("    .image { width:100% }\n")
		f.write("    .imageSmall { margin-top:10; margin-bottom:10; width:200 }\n")
		f.write("    .toc { margin-right:20; margin-bottom: 20; display:inline-block; }\n")
		f.write("</style>\n")
		f.write("</head>\n")
		f.write("<body class='body'>\n")
		f.write("\n")
		
		# Write the title.
		f.write("<div class='sectionTitle'>\n")
		f.write("<div class='title'>Media Review</div>\n")
		if "path" in data:
			f.write("<div class='textSmall'>" + data["path"] + "</div>\n")
		f.write("</div>\n")
		f.write("\n")
		
		# Write the summary.
		f.write("<div class='section'>")
		f.write("<div class='header'>Summary</div>\n")
		if "summary" in data:
			f.write("<div class='sectionSub'>\n")
			f.write("<div class='text'>" + data["summary"] + "</div>\n")
			f.write("</div>\n")
		split = splitFileName(imageSeq)
		if "frames" in data:
			f.write("<div class='sectionSub'>\n")
			j = 0
			for i in data["frames"]:
				f.write("<div class='toc'>")
				if "frameNumber" in i:
					f.write("<div class='textSmall'>Frame " + i["frameNumber"] + "</div>\n")
				f.write("<a href='#" + str(j) + "'>");
				f.write("<img class='imageSmall' src=\"" + split[1] + str(j) + split[3] + "\">")
				f.write("</a>\n")
				if "text" in i:
					f.write("<div class='textSmall'>" + elide(i["text"]) + "</div>\n")										
				f.write("</div>")
				j = j + 1
			f.write("</div>\n")
		f.write("</div>\n")
		f.write("\n")
		
		# Write each frame.
		if "frames" in data:
			j = 0
			for i in data["frames"]:
				f.write("<div class='section' id='" + str(j) + "'>\n")
				if "frameNumber" in i:
					f.write("<div class='header'>Frame " + i["frameNumber"] + "</div>\n")
				if "text" in i:
					f.write("<div class='sectionSub'>\n")
					f.write("<div class='text'>" + i["text"] + "</div>\n")				
					f.write("</div>\n")
				f.write("<div class='sectionSub'>\n")
				f.write("<img class='image' src=\"" + split[1] + str(j) + split[3] + "\">\n")
				f.write("</div>\n")
				f.write("</div>\n")
				f.write("\n")	
				j = j + 1

		# Write the footer.
		f.write("</body>\n")
		f.write("</html>\n")
		f.close()
		
		webbrowser.open(outputHTML, new=2)
		
except IOError:
	print("Error writing \"" + outputHTML + "\"")
	exit(1)
