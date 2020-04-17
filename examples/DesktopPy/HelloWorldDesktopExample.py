# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2019-2020 Darby Johnston
# All rights reserved.

import djvCorePy
import djvUIPy
import djvDesktopAppPy

import sys

try:
    # Create an application.
    app = djvDesktopAppPy.Application.create(sys.argv)
    
    # Create a label.
    label = djvUIPy.Label.create(app)
    label.setText("Hello world!")
    
    # Create a window and show it.
    window = djvUIPy.Window.create(app)
    window.addChild(label)
    window.show()
    
    # Run the application.
    app.run()

except Exception as e:
	print(str(e))

