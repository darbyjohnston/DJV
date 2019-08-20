import djvCorePy.FileSystem as fs
import djvCorePy.Frame as fr

import unittest

class FileInfoTest(unittest.TestCase):

    def test_Constructors(self):
        self.assertTrue(fs.FileInfo().isEmpty())
        self.assertFalse(fs.FileInfo(fs.Path("/")).isEmpty())
        self.assertFalse(fs.FileInfo(fs.Path("/"), fs.FileType.File).isEmpty())
        self.assertFalse(fs.FileInfo("/").isEmpty())
    
    def test_Members(self):
        self.assertEqual(fs.FileInfo("/").getPath(), fs.Path("/"))
        fi = fs.FileInfo()
        fi.setPath(fs.Path("/"))
        self.assertEqual(fi.getPath(), fs.Path("/"))
        fi.setPath(fs.Path("/"), fs.FileType.Directory)
        self.assertEqual(fi.getPath(), fs.Path("/"))
        self.assertEqual(fi.getType(), fs.FileType.Directory)
        fi.setPath(fs.Path("/tmp/file0001.ext"))
        self.assertEqual(fi.getFileName(), "/tmp/file0001.ext")
        fi.evalSequence()
        self.assertEqual(fi.getFileName(100), "/tmp/file0100.ext")
        self.assertEqual(fi.getFileName(100, False), "file0100.ext")

    def test_Sequences(self):
        fi = fs.FileInfo(fs.Path("/tmp/file1-3.ext"))
        self.assertFalse(fi.isSequenceValid())
        fi.evalSequence()
        self.assertTrue(fi.isSequenceValid())
        self.assertEqual(fi.getSequence(), fr.Sequence(fr.Range(1, 3)))
        fi.setSequence(fr.Sequence(fr.Range(3, 1), 4))
        fi.sortSequence()
        self.assertEqual(fi.getSequence(), fr.Sequence(fr.Range(1, 3), 4))
        self.assertFalse(fi.isSequenceWildcard())
        self.assertTrue(fs.FileInfo("/tmp/file#.ext").isSequenceWildcard())
        self.assertTrue(fi.isCompatible(fs.FileInfo("/tmp/file2.ext")))
        self.assertFalse(fi.isCompatible(fs.FileInfo("/tmp/render1.ext")))

    def test_Util(self):
        for f in fs.FileInfo.directoryList(fs.Path(".")):
            print(f.getFileName(fr.invalid, False))
    
if __name__ == '__main__':
    unittest.main()
