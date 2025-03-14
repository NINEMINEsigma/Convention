from ..Internal import *

# Import BaseClass.py
try:
    from pydantic   import BaseModel as _
    from ..Lang.BaseClass import *
except ImportError as ex:
    ImportingThrow(ex, "Lang-Base", ["pydantic"])

# Import CppLike.py
try:
    from ..Lang.CppLike import *
except ImportError as ex:
    ImportingThrow(ex, "CppLike", [])

# Import Reflection.py
try:
    import importlib as _
    import inspect as _
    from ..Lang.Reflection import *
except ImportError as ex:
    ImportingThrow(ex, "Reflection", ["importlib", "inspect"])