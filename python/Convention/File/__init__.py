from ..Internal import *

try:
    from ..Str import Core as _
    from ..Lang import *
except ImportError:
    InternalImportingThrow("File", ["Str", "Lang"])

# Import Core.py
try:
    import json as _
    import shutil as _
    import pandas as _
    import os as _
    import sys as _
    import pickle as _
    from pathlib                                        import Path as _
    from pydub                                          import AudioSegment as _
    from PIL                                            import Image as _, ImageFile as _
    from docx                                           import Document as _
    from docx.document                                  import Document as _
    from ..File.Core import *
except ImportError as ex:
    ImportingThrow(ex, "File Core", ["json", "shutil", "pandas", "os", "sys", "pickle", "pathlib", "pydub", "pillow", "python-docx"])