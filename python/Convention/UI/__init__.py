from ..Internal import *

# Import Tkinter.py
try:
    import tkinter as _
    from ..UI.Tkinter import *
except ImportError as ex:
    ImportingThrow(ex, "TkinterUI", ["tkinter"])

