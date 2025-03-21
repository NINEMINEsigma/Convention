from ..Internal import *

try:
    from ..MathEx import Core as _
except ImportError:
    InternalImportingThrow("Machine-Learning", ["Math"])

try:
    import sklearn as _
except ImportError as ex:
    ImportingThrow(ex, "Core", ["scikit-learn"])

try:
    from keras import api as _
    from ..ML.Keras import *
except ImportError as ex:
    ImportingThrow(ex, "Keras", ["tensorflow"])
