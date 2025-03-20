from ..Internal import *

try:
    from ..Lang.Reflection import light_reflection as _
except ImportError:
    InternalImportingThrow("String", ["Lang"])

# Import RE.py
try:
    import re as _
    from ..Str.RE import *
except ImportError:
    InternalImportingThrow("Regular-Expression", ["re"])

# Import Core.py
try:
    from pathlib import Path as _
    import re as _
    import xml.etree.ElementTree as _
    from xml.dom import minidom as _
    import math as _
    from ..Str.Core import *
except ImportError as ex:
    ImportingThrow(ex, "String-Core", ["pathlib", "re", "xml", "math"])