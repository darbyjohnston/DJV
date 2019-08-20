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
        self.app = c.Application.create(sys.argv)
        a = d.Animation.Animation.create(self.app)
        self.assertTrue(a != None)
        self.assertEqual(a.getType(), d.Animation.Type.Linear)
        a.setType(d.Animation.Type.Sine)
        self.assertEqual(a.getType(), d.Animation.Type.Sine)
        self.assertFalse(a.isRepeating())
        a.setRepeating(True)
        self.assertTrue(a.isRepeating())
        a.setRepeating(False)
        self.assertFalse(a.isActive())
        a.start(
            0.0,
            1.0,
            timedelta(seconds=1),
            self.animationValue,
            self.animationFinished)
        self.app.run()

if __name__ == '__main__':
    unittest.main()
