import djvCorePy as d

import sys
import unittest

class ContextTest(unittest.TestCase):

    def test_Context(self):
        c = d.Context.create(sys.argv)
        print("Args =", c.getArgs())
        print("Name =", c.getName())
    
if __name__ == '__main__':
    unittest.main()
