# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2020 Darby Johnston
# All rights reserved.

import djvCorePy.Frame as f

import unittest

class FrameTest(unittest.TestCase):

    def test_values(self):
        print("Invalid frame: ", f.invalid)
        print("Invalid range: ", f.invalidRange.min, f.invalidRange.max)

    def test_range(self):
        r = f.Range()
        self.assertEqual(r.min, 0)
        self.assertEqual(r.max, 0)
        r = f.Range(1, 10)
        self.assertEqual(r.min, 1)
        self.assertEqual(r.max, 10)
        r.zero()
        self.assertEqual(r.min, 0)
        self.assertEqual(r.max, 0)
        r = f.Range(1, 10)
        self.assertFalse(r.contains(0))
        self.assertTrue(r.contains(10))
        self.assertFalse(r.intersects(f.Range()))
        self.assertTrue(r.intersects(f.Range(1, 10)))
        r.expand(20)
        self.assertEqual(r.max, 20)
        r.expand(f.Range(0, 100))
        self.assertEqual(r.min, 0)
        self.assertEqual(r.max, 100)

    def test_sequence(self):
        self.assertFalse(f.Sequence().isValid())
        s = f.Sequence(f.Range(10, 1))
        self.assertEqual(len(s.ranges), 1)
        self.assertEqual(s.ranges[0].min, 10)
        self.assertEqual(s.ranges[0].max, 1)
        self.assertTrue(s.isValid())
        self.assertEqual(s.getSize(), 10)
        self.assertEqual(s.getFrame(0), 10)
        s.sort()
        self.assertEqual(s.ranges[0].min, 1)
        self.assertEqual(s.ranges[0].max, 10)

    def test_util(self):
        self.assertFalse(f.isValid(f.invalidRange))
        r = f.Range(10, 1)
        f.sort(r)
        self.assertEqual(r.min, 1)
        self.assertEqual(r.max, 10)

    def test_conversion(self):
        self.assertEqual(f.toFrames(f.Range(1, 3)), [1, 2, 3])
        self.assertEqual(f.toFrames(f.Sequence([f.Range(1, 3), f.Range(5, 6)])), [1, 2, 3, 5, 6])
        self.assertEqual(f.fromFrames([1, 2, 3]), f.Sequence(f.Range(1, 3)))
        self.assertEqual(f.toString(1), "1")
        self.assertEqual(f.toString(1, 4), "0001")
        self.assertEqual(f.toString(f.Range(1, 3)), "1-3")
        self.assertEqual(f.toString(f.Range(1, 3), 4), "0001-0003")
        self.assertEqual(f.toString(f.Sequence([f.Range(1, 3), f.Range(5, 6)])), "1-3,5-6")
        self.assertEqual(f.toString(f.Sequence([f.Range(1, 3), f.Range(5, 6)], 4)), "0001-0003,0005-0006")
        r = f.Range()
        pad = 0
        f.fromString("0001-0003", r, pad)
        self.assertEqual(r, f.Range(1, 3))
        #! Fix me!
        #self.assertEqual(pad, 4)
        s = f.Sequence()
        f.fromString("0001-0003,0005-0006", s)
        self.assertEqual(s, f.Sequence([f.Range(1, 3), f.Range(5, 6)], 4))
    
if __name__ == '__main__':
    unittest.main()
