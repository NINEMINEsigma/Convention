import time
import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from Convention.Lang.Reflection import *
from Convention.Lang.EasySave import *

def main():
    SetInternalDebug(True)
    from Convention.Workflow.Core import Workflow
    print(TypeManager.GetInstance().CreateOrGetRefType(Workflow).tree())

if __name__ == "__main__":
    main()