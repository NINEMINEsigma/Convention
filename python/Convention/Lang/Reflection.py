import importlib
import inspect
import types
from typing import *
from ..Internal import *
from pydantic import BaseModel, Field
type_symbols = {
    'int' : int,
    'float' : float,
    'str' : str,
    'list' : list,
    'dict' : dict,
    'tuple' : tuple,
    'set' : set,
    'bool' : bool,
    'NoneType' : type(None),
    }

def get_type_from_string(type_string:str) -> type:
        """根据字符串生成类型"""
        if type_string in type_symbols:
            return type_symbols[type_string]

        # 首先尝试从内置类型中获取
        if type_string in dir(types):
            return getattr(types, type_string)
        # 首先尝试从内置类型中获取
        elif type_string in globals():
            return globals().get(type_string)
        # 尝试从当前模块中获取
        elif type_string in dir(__import__(__name__)):
            return getattr(__import__(__name__), type_string)
        # 尝试从标准库中获取
        else:
            try:
                if '.' not in type_string:
                    raise ValueError(f"Empty module name, type_string is {type_string}")
                module_name, _, class_name = type_string.rpartition('.')
                if not module_name:
                    raise ValueError(f"Empty module name, type_string is {type_string}")
                module = importlib.import_module(module_name)
                return getattr(module, class_name)
            except (ImportError, AttributeError, ValueError) as ex:
                print("get_type_from_string failed.")
                print("first check in:{}".format(dir(types)))
                print("second check in:{}".format(globals()))
                print("third check in:{}".format(dir(__import__(__name__))))
                raise TypeError(f"Cannot find type '{type_string}', type_string is <{type_string}>") from ex

class light_reflection(any_class):
    def __init__(self, obj:object, type_str:str=None, *args, **kwargs):
        if obj is not None:
            self.obj = obj
        elif type_str is not None:
            self.obj = self.create_instance(type_str, args, kwargs)

    @override
    def SymbolName(self):
        return "light_reflection"
    @override
    def ToString(self):
        return f"ToolReflection<{type(self.obj).__name__}>"

    def get_attributes(self):
        """获取对象的所有属性和它们的值"""
        return {attr: getattr(self.obj, attr) for attr in dir(self.obj) if not callable(getattr(self.obj, attr)) and not attr.startswith("__")}

    def get_methods(self):
        """获取对象的所有方法"""
        return {method: getattr(self.obj, method) for method in dir(self.obj) if callable(getattr(self.obj, method)) and not method.startswith("__")}

    def contains_attribute(self, attr_name):
        """检查对象是否具有某个属性"""
        return hasattr(self.obj, attr_name)

    def contains_method(self, method_name):
        """检查对象是否具有某个方法"""
        return hasattr(self.obj, method_name) and callable(getattr(self.obj, method_name))

    def call_method(self, method_name, *args, **kwargs):
        """调用对象的方法"""
        if self.contains_method(method_name):
            return getattr(self.obj, method_name)(*args, **kwargs)
        else:
            raise AttributeError(f"{self.obj.__class__.__name__} object has no method '{method_name}'")

    def set_attribute(self, attr_name, value):
        """设置对象的属性值"""
        if self.contains_attribute(attr_name):
            setattr(self.obj, attr_name, value)
        else:
            raise AttributeError(f"{self.obj.__class__.__name__} object has no attribute '{attr_name}'")
    def set(self, field:str, value):
        self.set_attribute(field, value)

    def get_attribute(self, attr_name):
        """获取对象的属性值"""
        if self.contains_attribute(attr_name):
            return getattr(self.obj, attr_name)
        else:
            raise AttributeError(f"{self.obj.__class__.__name__} object has no attribute '{attr_name}'")
    def get(self, field:str):
        return self.get_attribute(field)

    def create_instance(self, type_string:str, *args, **kwargs):
        """根据类型字符串生成类型的实例"""
        type_ = get_type_from_string(type_string)
        return type_(*args, **kwargs)

    def create_instance_ex(self, type_string:str, params: Union[Dict[str,object], object]={}):
        """根据类型字符串生成类型的实例"""

        typen = get_type_from_string(type_string)
        if type_string in type_symbols:
            return typen(params)
        if params is None or len(params) == 0:
            return typen()

        # 获取构造函数参数信息
        constructor_params = inspect.signature(typen.__init__).parameters
        if len(constructor_params) == 0:
            return typen()

        # 准备构造函数参数
        init_args = {'args':None, 'kwargs':None}
        for param_name, param in constructor_params.items():
            if param_name == 'self':
                continue
            if param_name in params:
                init_args[param_name] = params[param_name]
            elif param.default is not param.empty:
                init_args[param_name] = param.default
            elif param_name == 'args' or param_name == 'kwargs':
                continue
            else:
                raise TypeError(f"Cannot instantiate type '{type_string}' without required parameter '{param_name}'")

        return typen(**init_args)

class MemberInfo(BaseModel, any_class):
    MemberName:   str = Field(default="", description="名称")
    ParentType:   type = Field(default_factory=type, description="所属类型")

class FieldInfo(BaseModel, any_class):
    FieldName:    str = Field(default="", description="字段名")
    FieldType:    type = Field(default_factory=type, description="字段类型")
    ParentType:   type = Field(default_factory=type, description="所属类型")

    def GetValue(self, obj:object) -> object:
        return getattr(obj, self.FieldName)
    def SetValue(self, obj:object, value:object) -> None:
        if not isinstance(value, self.FieldType):
            raise TypeError(f"Value type mismatch, expected {self.FieldType}, got {type(value)}")
        setattr(obj, self.FieldName, value)

class RefType(BaseModel, any_class):
    _RealType:      type = Field(default_factory=type, description="类型")
    _IsPrimitive:   bool = Field(default=False, description="是否为基本类型")
    _IsValueType:   bool = Field(default=False, description="是否为值类型")
    _IsCollection:  bool = Field(default=False, description="是否为容器")
    _IsDictionary:  bool = Field(default=False, description="是否为字典")
    _IsTuple:       bool = Field(default=False, description="是否为元组")
    _IsSet:         bool = Field(default=False, description="是否为集合")
    _IsList:        bool = Field(default=False, description="是否为列表")
    _IsUnsupported: bool = Field(default=False, description="是否为不支持的类型")

    _FieldInfo:     List[FieldInfo] = Field(default_factory=list, description="字段信息")

    @property
    def RealType(self) -> type:
        return self._RealType
    @property
    def IsCollection(self) -> bool:
        return self._IsCollection
    @property
    def IsPrimitive(self) -> bool:
        return self._IsPrimitive
    @property
    def IsValueType(self) -> bool:
        return self._IsValueType
    @property
    def IsDictionary(self) -> bool:
        return self._IsDictionary
    @property
    def IsTuple(self) -> bool:
        return self._IsTuple
    @property
    def IsSet(self) -> bool:
        return self._IsSet
    @property
    def IsList(self) -> bool:
        return self._IsList

    def __init__(self, type:type):
        self._RealType = type
        BaseModel.__init__(self)
        any_class.__init__(self)

        self._IsPrimitive = (
            issubclass(type, int) or
            issubclass(type, float) or
            issubclass(type, str) or
            issubclass(type, bool) or
            issubclass(type, complex) or
            issubclass(type, tuple) or
            issubclass(type, set) or
            issubclass(type, list) or
            issubclass(type, dict)
            )
        self._IsValueType = (
            issubclass(type, int) or
            issubclass(type, float) or
            issubclass(type, str) or
            issubclass(type, bool) or
            issubclass(type, complex)
            )
        self._IsCollection = (
            issubclass(type, list) or
            issubclass(type, dict) or
            issubclass(type, tuple) or
            issubclass(type, set)
            )
        self._IsDictionary = (
            issubclass(type, dict)
            )
        self._IsTuple = (
            issubclass(type, tuple)
            )
        self._IsSet = (
            issubclass(type, set)
            )
        self._IsList = (
            issubclass(type, list)
            )




_Internal_TypeManager:Optional['TypeManager'] = None

class TypeManager(BaseModel, any_class):
    _RefTypes:Dict[type, RefType] = Field(default_factory=dict, description="全体类型")

    @classmethod
    def GetInstance(cls) -> Self:
        global _Internal_TypeManager
        if _Internal_TypeManager is None:
            _Internal_TypeManager = cls()
        return _Internal_TypeManager

    def CreateOrGetRefType(self, type:type) -> RefType:
        if type in self._RefTypes:
            return self._RefTypes[type]
        else:
            ref_type = RefType(type)
            self._RefTypes[type] = ref_type
            return ref_type
