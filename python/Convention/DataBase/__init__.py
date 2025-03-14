from ..Internal import *
from abc import *

try:
    from ..Str import Core as _
except ImportError:
    InternalImportingThrow("DataBase", ["Str"])


# Import Core.py
try:
    from ..DataBase.Core import *
except ImportError as ex:
    ImportingThrow(ex, "DataBase Core", [])


# Import Core.py
try:
    import sqlite3 as _
    from ..DataBase.light_sqlite import *
except ImportError as ex:
    ImportingThrow(ex, "light_sqlite", ["sqlite3"])