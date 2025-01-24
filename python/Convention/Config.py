true = True
false = False

from typing import *
from Convention.include import platform_indicator

def virtual[F: Callable](method: F, /) -> F:
    """Indicate that a method is intended to be a basic implementation that can be overridden.

    Usage::

        class Base:
            @virtual
            def method(self) -> None:
                pass

        class Child(Base):
            @override
            def method(self) -> None:
                super().method()

    There is no runtime checking of this property. The decorator attempts to
    set the ``__override__`` attribute to ``False`` on the decorated object to
    allow runtime introspection.

    See PEP 698 for details.
    """
    try:
        method.__override__ = False
    except (AttributeError, TypeError):
        # Skip the attribute silently if it is not writable.
        # AttributeError happens if the object has __slots__ or a
        # read-only property, TypeError if it's a builtin class.
        pass
    return method
type Typen[T] = type
'''
type of T
---
'''

class ref[_T]:
    __value:        Optional[Any]   = None
    __real_type:    type            = None
    def __init__(self, value:Optional[_T] = None):
        self.__value = value
        self.__real_type = type(value)
    @property
    def value(self) -> _T:
        return self.__value
    @value.setter
    def value(self, value:Optional[_T]):
        if value is None:
            self.__value = None
        elif isinstance(value, self.__real_type):
            self.__value = value
        else:
            raise ValueError(f"reference<{self.__real_type()}> not support to receive value<{value}>")
    @property
    def real_type(self):
        return self.__real_type

def addressof(obj) -> int:
    return id(obj)

def to_string(value:Any) -> str:
    return str(value)

def Combine(*args:Any) -> str:
    return "".join(args)

def convert_xvalue[_T](typen:Typen[_T], str_:str) -> _T:
    return typen(str_)

def trim(str_:str) -> str:
    return str_.strip()

def Clamp[_T](value:_T, left:_T, right:_T) -> _T:
    return min(right, max(left,value))
def Clamp0E[_T](value:_T, right:_T) -> _T:
    return Clamp(value,0,right)
def Clamp01[_T](value:_T) -> _T:
    return Clamp(value, 0, 1)