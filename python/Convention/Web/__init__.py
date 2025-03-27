from ..Internal import *

try:
    from ..Str.Core import UnWrapper as _
    from ..File.Core import tool_file as _
    from ..MathEx.Transform import Rect as _
except ImportError:
    InternalImportingThrow("Web", ["String", "File", "Math"])

# Import Core.py
try:
    from http import server as _
    #from .Core import *
except ImportError as ex:
    ImportingThrow(ex, "Web-Core", ["http"])

# Import BeautifulSoup.py
try:
    import bs4 as _
    #from .BeautifulSoup import *
except ImportError as ex:
    ImportingThrow(ex, "BeautifulSoup", ["bs4"])

# Import Requests.py
try:
    import requests as _
    import urllib3 as _
    #from .Requests import *
except ImportError as ex:
    ImportingThrow(ex, "Requests", ["requests", "urllib3"])

# Import Selunit.py
try:
    import selenium as _
    #from .Selunit import *
except ImportError as ex:
    ImportingThrow(ex, "Selunit", ["selenium"])