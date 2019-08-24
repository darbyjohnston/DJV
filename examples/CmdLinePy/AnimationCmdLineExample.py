#------------------------------------------------------------------------------
# Copyright (c) 2019 Darby Johnston
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice,
#   this list of conditions, and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions, and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
# * Neither the names of the copyright holders nor the names of any
#   contributors may be used to endorse or promote products derived from this
#   software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#------------------------------------------------------------------------------

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
    app.exit()

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

except (error):
	print(error.what())

