
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from Convention.Engine.Internal import *

test = CObject()

print(test.GetAssembly())


