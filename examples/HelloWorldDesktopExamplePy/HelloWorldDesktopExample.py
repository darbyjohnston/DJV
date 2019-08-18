import djvCorePy
import djvUIPy
import djvDesktopAppPy
import sys

try:
	app = djvDesktopAppPy.Application.create(sys.argv)

	window = djvUIPy.Window.create(app)
	window.show()

	app.run()

except (error):
	print(error.what())
