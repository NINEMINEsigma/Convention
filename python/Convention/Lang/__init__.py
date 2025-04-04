from ..Internal import *

# Import CppLike.py
try:
    pass
    #from ..Lang.CppLike import *
except ImportError as ex:
    ImportingThrow(ex, "CppLike", [])

# Import Reflection.py
try:
    import importlib as _
    import inspect as _
    #from ..Lang.Reflection import *
except ImportError as ex:
    ImportingThrow(ex, "Reflection", ["importlib", "inspect"])