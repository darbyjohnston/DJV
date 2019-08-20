import djvCorePy.FileSystem as fs

import unittest

class FileInfoTest(unittest.TestCase):

    def test_Constructors(self):
        self.assertTrue(fs.FileInfo().isEmpty())
        self.assertFalse(fs.FileInfo(fs.Path("/")).isEmpty())
        self.assertFalse(fs.FileInfo(fs.Path("/"), fs.FileType.File).isEmpty())
        self.assertFalse(fs.FileInfo("/").isEmpty())
    
    def test_Members(self):
        self.assertTrue(fs.FileInfo("/").getPath().__eq__(fs.Path("/")))
        fi = fs.FileInfo()
        fi.setPath(fs.Path("/"))
        self.assertTrue(fi.getPath().__eq__(fs.Path("/")))
        fi.setPath(fs.Path("/"), fs.FileType.Directory)
        self.assertTrue(fi.getPath().__eq__(fs.Path("/")))
        self.assertEqual(fi.getType(), fs.FileType.Directory)
        fi.setPath(fs.Path("/tmp/file0001.ext"))
        self.assertEqual(fi.getFileName(), "/tmp/file0001.ext")
        #self.assertEqual(fi.getFileName(100), "/tmp/file0100.ext")
        #self.assertEqual(fi.getFileName(100, False), "file0100.ext")

if __name__ == '__main__':
    unittest.main()
