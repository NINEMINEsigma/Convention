from typing import *

verbose_auto                = Literal["auto"]
verbose_silent              = Literal[0]
verbose_progress_bar        = Literal[1]
verbose_one_line_per_epoch  = Literal[2]
verbose_type                = Literal[
    "auto", 0, 1, 2,
    verbose_auto, verbose_silent,
    verbose_progress_bar, verbose_one_line_per_epoch
    ]



