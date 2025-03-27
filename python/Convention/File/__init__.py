from ..Internal import *

try:
    from ..Str import Core as _
    from ..Lang import *
except ImportError:
    InternalImportingThrow("File", ["Str", "Lang"])

# Import Core.py
try:
    import shutil as _
    import pandas as _
    import pickle as _
    from pathlib                                        import Path as _
    from pydub                                          import AudioSegment as _
    from PIL                                            import Image as _, ImageFile as _
    from docx                                           import Document as _
    from docx.document                                  import Document as _
    #from ..File.Core import *
except ImportError as ex:
    ImportingThrow(ex, "File Core", ["shutil", "pandas", "pathlib", "pydub", "pillow", "python-docx"])