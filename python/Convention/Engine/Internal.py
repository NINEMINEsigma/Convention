from ..Internal import *

class CObject(BaseModel, any_class):
    def __init__(self, *args, **kwargs):
        BaseModel.__init__(self, *args, **kwargs)
        any_class.__init__(self)
    
    _CachedPtr:    int     = PrivateAttr(default=-1)
    _InstanceID:   int     = PrivateAttr(default=-1)
    _EngineRuntimeErrorString:              str = PrivateAttr(default="")
    _OffsetOfInstanceIDInCPlusPlusObject:   int = PrivateAttr(default=-1)
    _ObjectIsNullMessage:                   str = PrivateAttr(default="The Object you want to instantiate is null.")
    _CloneDestroyedMessage:                 str = PrivateAttr(default="Instantiate failed because the clone was destroyed during creation. This can happen if DestroyImmediate is called in MonoBehaviour.Awake.")
    
    @override
    def GetAssembly(self) -> str:
        return "Engine"
    @override
    def GetSymbolName(self) -> str:
        return "Object"
    @override
    def ToString(self) -> str:
        return self.GetType().__name__
    
    @classmethod
    def class_name(cls) -> str:
        return cls.__name__
    
    @override
    def __str__(self) -> str:
        return self.ToString()
    
    
