
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from Convention.File.Core import *

def run():
    data = tool_file("./tests/text.txt").load()
    
    print_colorful(ConsoleFrontColor.RED, data)

if __name__ == "__main__":
    run()

