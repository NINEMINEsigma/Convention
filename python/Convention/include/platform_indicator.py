from types import *
from typing import *
import platform

type tag = NoneType
is_release = True
value = is_release
is_platform_windows = platform.system().lower().startswith("windows")
is_platform_linux = platform.system().lower().startswith("linux")
is_platform_x64 = platform.machine().lower().startswith("x64")
is_platform_x86 = platform.machine().lower().startswith("x86")
is_platform_x86_x64 = platform.machine().lower().startswith("x86_x64")
is_platform_i386 = platform.machine().lower().startswith("i386")
is_msvc = False

def generate_platform_message():
    return platform.platform()