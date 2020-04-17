# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2020 Darby Johnston
# All rights reserved.

import djvCorePy.FileSystem as fs
import djvCorePy.Frame as fr

import unittest

class FileInfoTest(unittest.TestCase):

    def test_ctor(self):
        self.assertTrue(fs.FileInfo().isEmpty())
        self.assertFalse(fs.FileInfo(fs.Path("/")).isEmpty())
        self.assertFalse(fs.FileInfo(fs.Path("/"), fs.FileType.File).isEmpty())
        self.assertFalse(fs.FileInfo("/").isEmpty())
    
    def test_util(self):
        for f in fs.FileInfo.directoryList(fs.Path(".")):
            print(f.getFileName(fr.invalid, False))
    
if __name__ == '__main__':
    unittest.main()
