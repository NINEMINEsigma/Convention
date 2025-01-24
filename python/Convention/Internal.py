from typing import *
from types import *
import inspect
from pydantic import BaseModel
from Convention.Config import *

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
    
    def __str__(self):
        return self.ToString()
    
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
            