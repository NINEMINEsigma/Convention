import time
import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from Convention.Lang.Reflection import *
from Convention.Lang.EasySave import *

class A(BaseModel):
    a:int = Field(description="a")
class B(A):
    b:int = Field(description="b")
class D(B, any_class):
    d:int = Field(description="d")

def main():
    print(D.__bases__)
    print(D.__bases__[0])

if __name__ == "__main__":
    main()



