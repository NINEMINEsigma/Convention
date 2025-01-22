true = True
false = False

from typing import *
from Convention.include import platform_indicator

def addressof(obj) -> int:
    return id(obj)