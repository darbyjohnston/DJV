# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2020 Darby Johnston
# All rights reserved.

import djvCorePy as d
import djvCmdLineAppPy as c

import sys
import unittest
from datetime import timedelta

class AnimationTest(unittest.TestCase):

    app = None

    def animationValue(self, v):
        buf = ""
        for i in range(0, int(v * 40.0)):
            buf += "#"
        print(buf)
    def animationFinished(self, v):
        print(v)
        self.app.exit()

    def test_Animation(self):
        for i in [
            d.Animation.Type.Linear,
            d.Animation.Type.EaseIn,
            d.Animation.Type.EaseOut,
            d.Animation.Type.SmoothStep,
            d.Animation.Type.Sine]:
            self.app = c.Application.create(sys.argv)
            a = d.Animation.Animation.create(self.app)
            self.assertFalse(a.isActive())
            self.assertFalse(a.isRepeating())
            a.setType(i)
            a.setRepeating(True)
            self.assertEqual(a.getType(), i)
            self.assertTrue(a.isRepeating())
            a.start(
                0.0,
                1.0,
                timedelta(seconds=1),
                self.animationValue,
                self.animationFinished)
            self.app.run()

if __name__ == '__main__':
    unittest.main()
