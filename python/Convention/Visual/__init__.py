from ..Internal import *

try:
    from ..File.Core        import tool_file as _
    from ..MathEx.Core      import np as _
except ImportError:
    InternalImportingThrow("Visual", ["File", "MathEx"])

# Import Core.py
try:
    import matplotlib.pyplot    as     _
    import seaborn              as     _
    import pandas               as     _
    import cv2                  as _
    import numpy                as _
    from ..Visual.Core import *
except ImportError as ex:
    ImportingThrow(ex, "Visual-Core", ["matplotlib", "seaborn", "pandas", "opencv-python", "numpy"])

# Import OpenCV.py
try:
    import numpy            as     _
    from PIL                import ImageFile as _
    from ..Visual.OpenCV import *
except ImportError as ex:
    ImportingThrow(ex, "OpenCV", ["numpy", "pillow"])

# Import WordCloud.py
try:
    from pyecharts.charts import WordCloud as _
    from pyecharts import options as _
    from ..Visual.WordCloud import *
except ImportError as ex:
    ImportingThrow(ex, "WordCloud", ["wordcloud"])

# Import Manim.py
try:
    import manim as _
    from ..Visual.Manim import *
except ImportError as ex:
    ImportingThrow(ex, "Manim", ["manim"])