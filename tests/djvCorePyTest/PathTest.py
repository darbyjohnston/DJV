# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2020 Darby Johnston
# All rights reserved.

import djvCorePy.FileSystem as fs

import unittest

class PathTest(unittest.TestCase):

    def test_ctor(self):
        self.assertTrue(fs.Path().isEmpty())
        self.assertEqual(fs.Path("/").get(), "/")
        self.assertEqual(fs.Path(fs.Path("/"), "a").get(), "/a")
        self.assertEqual(fs.Path("/", "a").get(), "/a")

    def test_members(self):
        path = fs.Path()
        path.set("/")
        self.assertEqual(path.get(), "/")
        path.append("a")
        self.assertEqual(path.get(), "/a")

    def test_root(self):
        self.assertFalse(fs.Path("").isRoot())
        self.assertTrue(fs.Path("/").isRoot())
        self.assertTrue(fs.Path("C:/").isRoot())
    
    def test_cd(self):
        path = fs.Path("/a/b")
        path.cdUp()
        self.assertEqual(path.get(), fs.Path.getCurrentSeparator() + "a")
    
    def test_components(self):
        path = fs.Path("/a/b1.ext")
        self.assertEqual(path.getFileName(), "b1.ext")
        self.assertEqual(path.getBaseName(), "b")
        self.assertEqual(path.getNumber(), "1")
        self.assertEqual(path.getExtension(), ".ext")
    
    def test_util(self):
        self.assertFalse(fs.Path.isSeparator(' '))
        self.assertTrue(fs.Path.isSeparator('/'))
        self.assertTrue(fs.Path.isSeparator('\\'))
        for i in [fs.PathSeparator.Unix, fs.PathSeparator.Windows]:
            print(i, "=", fs.Path.getSeparator(i))
        print("Current path separator =", fs.Path.getCurrentSeparator())
        print("Absolute path =", fs.Path.getAbsolute(fs.Path(".")).get())
        print("Current working directory =", fs.Path.getCWD().get())
        print("Temp file path =", fs.Path.getTemp().get())
        
if __name__ == '__main__':
    unittest.main()
