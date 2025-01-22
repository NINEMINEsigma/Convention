from typing import *
from types import *
import inspect
from pydantic import BaseModel
from Convention.Config import *

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
    def __init  (self, value:Optional[_T] = None):
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
class type_class:
    @virtual
    def GetType(self):
        return type(self)
    @virtual
    def ToString(self) -> str:
        return self.__class__.__name__
    @virtual
    def SymbolName(self) -> str:
        return str(self.GetType())
    @virtual
    def Is(self, typen:type) -> bool:
        return isinstance(self, typen)
    def As[_T](self, typen:Typen[_T]) -> Optional[_T]:
        if isinstance(self, typen):
            return self
        else:
            check_name = "__"+typen.__name__+"__"
            if check_name in dir(self):
                return getattr(self, check_name)()
            try:
                return typen(self)
            except Exception as ex:
                pass
        return None
    
    def IfIam[_T](
        self,
        typen:  Typen[_T],
        call:   Callable[[Union[_T, NoneType]], None]
        ) -> Self:
        if self.Is(typen):
            if len(inspect.signature(call).parameters)==0:
                call()
            else:
                call(self)
        return self
    def Fetch[_T](self, from_:_T) -> _T:
        return from_
    def Share[_T](self, refer:ref[_T]) -> Self:
        refer.value=self
        return self
class any_class(type_class):
    pass
            