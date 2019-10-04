import djvCorePy as d

import sys
import unittest

class ContextTest(unittest.TestCase):

    def test_Context(self):
        c = d.Context.create(sys.argv)
        print("args: ", c.getArgs())
        print("name: ", c.getName())
    
if __name__ == '__main__':
    unittest.main()
