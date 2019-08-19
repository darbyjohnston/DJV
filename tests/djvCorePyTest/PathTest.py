import djvCorePy

import unittest

class TestStringMethods(unittest.TestCase):

    def test_Constructors(self):
        self.assertTrue(djvCorePy.FileSystem.Path().isEmpty())

if __name__ == '__main__':
    unittest.main()
