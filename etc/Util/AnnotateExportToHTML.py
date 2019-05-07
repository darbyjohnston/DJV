import json
import sys

def elide(value, max=30):
	if len(value) > max:
		value = value[0:max] + "..."
	return value

# Parse the command line arguments.
if len(sys.argv) < 3:
	print("Usage: AnnotateExportToHTML.py (output HTML file) (input JSON file) [input image]...")
	exit(1)
outputHTML = sys.argv[1]
inputJSON = sys.argv[2]
images = []
for i in range(3, len(sys.argv)):
	images.append(sys.argv[i])

# Read the input JSON file.
data = {}
try:
	with open(inputJSON, "r") as f:
		data = json.load(f)
except IOError:
	print("Error reading \"" + inputJSON + "\"")
	exit(1)

# Write the output HTML file.
try:
	with open(outputHTML, "w") as f:
	
		# Write the header.
		f.write("<html>\n")
		f.write("<head>\n")
		f.write("<style>\n")
		f.write("    .body { margin:0; color:#ffffff; background-color:#223344; font-family:sans-serif; }\n")
		f.write("    .title { font-size:48; }\n")
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
		if "frames" in data:
			f.write("<div class='sectionSub'>\n")
			j = 0
			for i in data["frames"]:
				if j < len(images):
					f.write("<div class='toc'>")
					if "frameNumber" in i:
						f.write("<div class='textSmall'>Frame " + i["frameNumber"] + "</div>\n")
					f.write("<a href='#" + str(j) + "'>");
					f.write("<img class='imageSmall' src=\"" + images[j] + "\">")
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
				if j < len(images):
					f.write("<div class='sectionSub'>\n")
					f.write("<img class='image' src=\"" + images[j] + "\">\n")
					f.write("</div>\n")
				f.write("</div>\n")
				f.write("\n")	
				j = j + 1

		# Write the footer.
		f.write("</body>\n")
		f.write("</html>\n")
		f.close()
		
except IOError:
	print("Error writing \"" + outputHTML + "\"")
	exit(1)
