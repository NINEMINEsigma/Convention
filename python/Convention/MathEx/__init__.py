from ..Internal import *

try:
    import numpy                        as     _
    import scipy                        as     _
    from scipy          import optimize as     _
except ImportError as ex:
    ImportingThrow(ex, "Math-Ex", ["numpy", "scipy"])