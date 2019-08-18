import djvCorePy
import djvUIPy
import djvDesktopAppPy
import sys

try:
	app = djvDesktopAppPy.Application.create(sys.argv)
    
	label = djvUIPy.Label.create(app)
	label.setText("Hello world!")
    
	window = djvUIPy.Window.create(app)
	window.addChild(label)
	window.show()
    
	app.run()

except (error):
	print(error.what())

