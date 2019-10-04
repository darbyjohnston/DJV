import djvCorePy as d

import unittest

class BBoxTest(unittest.TestCase):

    def test_ivec2(self):
        v = d.ivec2(1, 2)
        self.assertTrue(v.x, 1)
        self.assertTrue(v.y, 2)
        v.x = 3
        v.y = 4
        self.assertTrue(v.x, 3)
        self.assertTrue(v.y, 4)

    def test_vec2(self):
        v = d.vec2(1.0, 2.0)
        self.assertTrue(v.x, 1.0)
        self.assertTrue(v.y, 2.0)
        v.x = 3.0
        v.y = 4.0
        self.assertTrue(v.x, 3.0)
        self.assertTrue(v.y, 4.0)
    
if __name__ == '__main__':
    unittest.main()
