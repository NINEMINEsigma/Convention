import importlib
import inspect
import types
from typing import *
from ..Internal import *
from pydantic import BaseModel, Field, PrivateAttr
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
    _MemberName:    str  = PrivateAttr(default="")
    _ParentType:    type = PrivateAttr(default_factory=type)
    _IsStatic:      bool = PrivateAttr(default=False)
    _IsPublic:      bool = PrivateAttr(default=False)

    def __init__(self, name:str, ctype:type, is_static:bool=False, is_public:bool=True):
        BaseModel.__init__(self)
        any_class.__init__(self)
        self._MemberName = name
        self._ParentType = ctype
        self._IsStatic = is_static
        self._IsPublic = is_public

    @property
    def MemberName(self) -> str:
        return self._MemberName
    @property
    def ParentType(self) -> type:
        return self._ParentType
    @property
    def IsStatic(self) -> bool:
        return self._IsStatic
    @property
    def IsPublic(self) -> bool:
        return self._IsPublic

class ValueInfo(BaseModel, any_class):
    _RealType:      Optional[type]  = PrivateAttr(default=None)
    _IsPrimitive:   bool            = PrivateAttr(default=False)
    _IsValueType:   bool            = PrivateAttr(default=False)
    _IsCollection:  bool            = PrivateAttr(default=False)
    _IsDictionary:  bool            = PrivateAttr(default=False)
    _IsTuple:       bool            = PrivateAttr(default=False)
    _IsSet:         bool            = PrivateAttr(default=False)
    _IsList:        bool            = PrivateAttr(default=False)
    _IsUnsupported: bool            = PrivateAttr(default=False)

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
    @property
    def IsUnsupported(self) -> bool:
        return self._IsUnsupported

    def __init__(self, type_:type):
        BaseModel.__init__(self)
        any_class.__init__(self)
        self._RealType = type_
        self._IsPrimitive = (
            issubclass(type_, int) or
            issubclass(type_, float) or
            issubclass(type_, str) or
            issubclass(type_, bool) or
            issubclass(type_, complex) or
            issubclass(type_, tuple) or
            issubclass(type_, set) or
            issubclass(type_, list) or
            issubclass(type_, dict)
            )
        self._IsValueType = (
            issubclass(type_, int) or
            issubclass(type_, float) or
            issubclass(type_, str) or
            issubclass(type_, bool) or
            issubclass(type_, complex)
            )
        self._IsCollection = (
            issubclass(type_, list) or
            issubclass(type_, dict) or
            issubclass(type_, tuple) or
            issubclass(type_, set)
            )
        self._IsDictionary = (
            issubclass(type_, dict)
            )
        self._IsTuple = (
            issubclass(type_, tuple)
            )
        self._IsSet = (
            issubclass(type_, set)
            )
        self._IsList = (
            issubclass(type_, list)
            )

class FieldInfo(MemberInfo, ValueInfo):

    def __init__(self, fieldType:type, name:str, ctype:type, is_static:bool, is_public:bool):
        MemberInfo.__init__(self, name, ctype, is_static, is_public)
        ValueInfo.__init__(self, fieldType)

    @property
    def FieldName(self) -> str:
        '''
        字段名称
        '''
        return self.MemberName
    @property
    def FieldType(self) -> type:
        '''
        字段类型
        '''
        return self.RealType

    @virtual
    def GetValue(self, obj:object) -> object:
        if self.IsStatic:
            return getattr(self.ParentType, self.MemberName)
        else:
            if not isinstance(obj, self.ParentType):
                raise TypeError(f"Parent type mismatch, expected {self.ParentType}, got {type(obj)}")
            return getattr(obj, self.MemberName)
    @virtual
    def SetValue(self, obj:object, value:object) -> None:
        if self.IsStatic:
            setattr(self.ParentType, self.MemberName, value)
        else:
            if not isinstance(obj, self.ParentType):
                raise TypeError(f"Parent type mismatch, expected {self.ParentType}, got {type(obj)}")
            if not isinstance(value, self.RealType):
                raise TypeError(f"Value type mismatch, expected {self.RealType}, got {type(value)}")
            setattr(obj, self.MemberName, value)

class ParameterInfo(ValueInfo):
    _ParameterName: str  = PrivateAttr(default="")
    _IsOptional:    bool = PrivateAttr(default=False)
    _DefaultValue:  Any  = PrivateAttr(default=None)

    def __init__(self, type:type, name:str, is_optional:bool, default_value:Any):
        ValueInfo.__init__(self, type)
        self._ParameterName = name
        self._IsOptional = is_optional
        self._DefaultValue = default_value

    @property
    def ParameterName(self) -> str:
        return self._ParameterName
    @property
    def IsOptional(self) -> bool:
        return self._IsOptional
    @property
    def DefaultValue(self) -> Any:
        return self._DefaultValue

class MethodInfo(MemberInfo):
    _ReturnType:            type                = PrivateAttr(default_factory=type)
    _Parameters:            List[ParameterInfo] = PrivateAttr(default_factory=list)
    _PositionalParameters:  List[ParameterInfo] = PrivateAttr(default_factory=list)
    _KeywordParameters:     List[ParameterInfo] = PrivateAttr(default_factory=list)

    def __init__(
        self,
        return_type:            type,
        parameters:             List[ParameterInfo],
        positional_parameters:  List[ParameterInfo],
        keyword_parameters:     List[ParameterInfo],
        name:                   str,
        ctype:                  type,
        is_static:              bool,
        is_public:              bool,
        ):
        MemberInfo.__init__(self, name, ctype, is_static, is_public)
        self._ReturnType = return_type
        self._Parameters = parameters
        self._PositionalParameters = positional_parameters
        self._KeywordParameters = keyword_parameters

    @property
    def ReturnType(self) -> type:
        return self._ReturnType
    @property
    def Parameters(self) -> List[ParameterInfo]:
        return self._Parameters
    @property
    def PositionalParameters(self) -> List[ParameterInfo]:
        return self._PositionalParameters
    @property
    def KeywordParameters(self) -> List[ParameterInfo]:
        return self._KeywordParameters

class RefType(ValueInfo):
    _FieldInfos:    List[FieldInfo]  = PrivateAttr(default_factory=list)
    _MethodInfos:   List[MethodInfo] = PrivateAttr(default_factory=list)

    def __init__(self, type:type):
        super().__init__(type)
        self._FieldInfos = []
        self._MethodInfos = []

        # 获取所有成员变量
        for name, value in type.__dict__.items():
            if not name.startswith('_'):
                field_type = type(value)
                is_static = isinstance(value, (int, float, str, bool, complex, list, dict, tuple, set))
                field_info = FieldInfo(field_type, name, type, is_static, True)
                self._FieldInfos.append(field_info)

        # 获取所有方法
        for name, method in inspect.getmembers(type):
            if not name.startswith('_') and inspect.isfunction(method):
                # 获取方法签名
                sig = inspect.signature(method)

                # 构建参数列表
                parameters = []
                positional_parameters = []
                keyword_parameters = []

                for param_name, param in sig.parameters.items():
                    if param_name == 'self':
                        continue

                    param_info = ParameterInfo(
                        param.annotation if param.annotation != inspect.Parameter.empty else Any,
                        param_name,
                        param.default != inspect.Parameter.empty,
                        param.default if param.default != inspect.Parameter.empty else None
                    )

                    parameters.append(param_info)
                    if param.kind == inspect.Parameter.POSITIONAL_ONLY or param.kind == inspect.Parameter.POSITIONAL_OR_KEYWORD:
                        positional_parameters.append(param_info)
                    elif param.kind == inspect.Parameter.KEYWORD_ONLY:
                        keyword_parameters.append(param_info)

                # 构建方法信息
                method_info = MethodInfo(
                    sig.return_annotation if sig.return_annotation != inspect.Signature.empty else Any,
                    parameters,
                    positional_parameters,
                    keyword_parameters,
                    name,
                    type,
                    False,
                    True
                )
                self._MethodInfos.append(method_info)


    def GetFields(self) -> List[FieldInfo]:
        return self._FieldInfos
    def GetMethods(self) -> List[MethodInfo]:
        return self._MethodInfos
    def GetMembers(self) -> List[MemberInfo]:
        return self._FieldInfos + self._MethodInfos


_Internal_TypeManager:Optional['TypeManager'] = None

class TypeManager(BaseModel, any_class):
    _RefTypes:Dict[type, RefType] = PrivateAttr(default_factory=dict)

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
