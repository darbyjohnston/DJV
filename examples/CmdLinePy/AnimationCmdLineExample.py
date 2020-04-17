# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2019-2020 Darby Johnston
# All rights reserved.

import djvCorePy
import djvCmdLineAppPy

import sys
from datetime import timedelta

app = None

def animationValue(v):
    buf = ""
    for i in range(0, int(v * 40.0)):
        buf += "#"
    print(buf)
def animationFinished(v):
    app.exit(0)

try:
    # Create an application.
	app = djvCmdLineAppPy.Application.create(sys.argv)
    
	# Create an animation.
	animation = djvCorePy.Animation.Animation.create(app)
	animation.setType(djvCorePy.Animation.Type.Sine)
	animation.start(
		0.0,
		1.0,
		timedelta(seconds=1),
		animationValue,
		animationFinished)
    
	# Run the application.
	app.run()

except Exception as e:
	print(str(e))

