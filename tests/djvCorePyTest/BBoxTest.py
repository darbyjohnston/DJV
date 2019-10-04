import djvCorePy as d

import unittest

class BBoxTest(unittest.TestCase):

    def test_ctor2i(self):
        b = d.BBox2i(d.ivec2(0, 1))
        self.assertEqual(b.min, d.ivec2(0, 1))
        self.assertEqual(b.max, d.ivec2(0, 1))
        b = d.BBox2i(d.ivec2(0, 1), d.ivec2(2, 3))
        self.assertEqual(b.min, d.ivec2(0, 1))
        self.assertEqual(b.max, d.ivec2(2, 3))
        b = d.BBox2i(0, 1, 2, 3)
        self.assertEqual(b.min, d.ivec2(0, 1))
        self.assertEqual(b.max, d.ivec2(1, 3))

    def test_ctor2f(self):
        b = d.BBox2f(d.vec2(0.0, 1.0))
        self.assertEqual(b.min, d.vec2(0.0, 1.0))
        self.assertEqual(b.max, d.vec2(0.0, 1.0))
        b = d.BBox2f(d.vec2(0.0, 1.0), d.vec2(2.0, 3.0))
        self.assertEqual(b.min, d.vec2(0.0, 1.0))
        self.assertEqual(b.max, d.vec2(2.0, 3.0))
        b = d.BBox2f(0.0, 1.0, 2.0, 3.0)
        self.assertEqual(b.min, d.vec2(0.0, 1.0))
        self.assertEqual(b.max, d.vec2(2.0, 4.0))

    def test_components2i(self):
        b = d.BBox2i(0, 1, 2, 3)
        self.assertEqual(b.x(), 0)
        self.assertEqual(b.y(), 1)
        self.assertEqual(b.w(), 2)
        self.assertEqual(b.h(), 3)
        self.assertTrue(b.isValid())
        b.max = d.ivec2(0, 0)
        self.assertFalse(b.isValid())
        b = d.BBox2i(0, 1, 2, 4)
        self.assertEqual(d.ivec2(2, 4), b.getSize())
        self.assertEqual(d.ivec2(1, 3), b.getCenter())
        self.assertEqual(8, b.getArea())
        self.assertEqual(0.5, b.getAspect())
        b.zero()
        self.assertEqual(d.ivec2(0, 0), b.min)
        self.assertEqual(d.ivec2(0, 0), b.max)

    def test_components2f(self):
        b = d.BBox2f(0.0, 1.0, 2.0, 3.0)
        self.assertEqual(b.x(), 0.0)
        self.assertEqual(b.y(), 1.0)
        self.assertEqual(b.w(), 2.0)
        self.assertEqual(b.h(), 3.0)
        self.assertTrue(b.isValid())
        b.max = d.vec2(0.0, 0.0)
        self.assertFalse(b.isValid())
        b = d.BBox2f(0.0, 1.0, 2.0, 4.0)
        self.assertEqual(d.vec2(2.0, 4.0), b.getSize())
        self.assertEqual(d.vec2(1.0, 3.0), b.getCenter())
        self.assertEqual(8.0, b.getArea())
        self.assertEqual(0.5, b.getAspect())
        b.zero()
        self.assertEqual(d.vec2(0, 0), b.min)
        self.assertEqual(d.vec2(0, 0), b.max)

    def test_util2i(self):
        a = d.BBox2i(0, 1, 2, 3)
        #self.assertTrue(a.contains(d.BBox2i(0, 1, 1, 1)))
        #self.assertFalse(a.contains(d.BBox2i(3, 5, 1, 1)))
        #self.assertTrue(a.contains(d.ivec2(0, 1)))
        #self.assertFalse(a.contains(d.ivec2(0, 0)))
        self.assertTrue(a.intersects(d.BBox2i(0, 1, 3, 4)))
        b = a.intersect(d.BBox2i(1, 2, 3, 4))
        self.assertEqual(b, d.BBox2i(1, 2, 1, 2))
        #a.expand(d.BBox2i(1, 2, 3, 4))
        #self.assertEqual(a, d.BBox2i(0, 1, 4, 5))
        #a.expand(d.ivec2(4, 6))
        #self.assertEqual(a, d.BBox2i(0, 1, 5, 6))
        #a = a.margin(d.ivec2(1, 2))
        #self.assertEqual(a, d.BBox2i(-1, -1, 7, 10))
        #a = a.margin(-1)
        #self.assertEqual(a, d.BBox2i(0, 0, 5, 8))
        #a = a.margin(1, 2, 3, 4)
        #self.assertEqual(a, d.BBox2i(-1, -2, 9, 14))

    def test_util2f(self):
        a = d.BBox2f(0.0, 1.0, 2.0, 3.0)
        #self.assertTrue(a.contains(d.BBox2f(0.0, 1.0, 1.0, 1.0)))
        #self.assertFalse(a.contains(d.BBox2f(3.0, 5.0, 1.0, 1.0)))
        #self.assertTrue(a.contains(d.vec2(0.0, 1.0)))
        #self.assertFalse(a.contains(d.vec2(0.0, 0.0)))
        self.assertTrue(a.intersects(d.BBox2f(0.0, 1.0, 3.0, 4.0)))
        b = a.intersect(d.BBox2f(1.0, 2.0, 3.0, 4.0))
        self.assertEqual(b, d.BBox2f(1.0, 2.0, 1.0, 2.0))
        #a.expand(d.BBox2f(1.0, 2.0, 3.0, 4.0))
        #self.assertEqual(a, d.BBox2f(0.0, 1.0, 4.0, 5.0))
        #a.expand(d.vec2(4.0, 6.0))
        #self.assertEqual(a, d.BBox2f(0.0, 1.0, 5.0, 6.0))
        #a = a.margin(d.vec2(1.0, 2.0))
        #self.assertEqual(a, d.BBox2f(-1.0, -1.0, 7.0, 10.0))
        #a = a.margin(-1.0)
        #self.assertEqual(a, d.BBox2f(0.0, 0.0, 5.0, 8.0))
        #a = a.margin(1.0, 2.0, 3.0, 4.0)
        #self.assertEqual(a, d.BBox2f(-1.0, -2.0, 9.0, 14.0))

    def test_util2i_comparison(self):
        a = d.BBox2i(0, 1, 2, 3)
        b = d.BBox2i(0, 0, 0, 0)
        self.assertEqual(a, a)
        self.assertNotEqual(a, b)
    
    def test_util2f_comparison(self):
        a = d.BBox2f(0.0, 1.0, 2.0, 3.0)
        b = d.BBox2f(0.0, 0.0, 0.0, 0.0)
        self.assertEqual(a, a)
        self.assertNotEqual(a, b)
    
if __name__ == '__main__':
    unittest.main()
