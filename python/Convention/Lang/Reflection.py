import                  importlib
import                  inspect
import                  types
from enum        import Enum, IntFlag
from typing      import *
from ..Internal  import *
from pydantic    import BaseModel, Field, PrivateAttr
import                  json
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

class ReflectionException(Exception):
    def __init__(self, message:str):
        self.message = message
        super().__init__(self.message)

def get_type_from_string(type_string:str) -> type:
        """
        根据字符串生成类型
        """
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
                #print("get_type_from_string failed.")
                #print("first check in:{}".format(dir(types)))
                #print("second check in:{}".format(globals()))
                #print("third check in:{}".format(dir(__import__(__name__))))
                raise TypeError(f"Cannot find type '{type_string}', type_string is <{type_string}>") from ex

def get_type_from_string_with_module(type_string:str, module_name:str) -> type:
    '''
    根据字符串生成类型
    '''
    if type_string in type_symbols:
        return type_symbols[type_string]
    elif type_string in dir(__import__(module_name)):
        return getattr(__import__(module_name), type_string)
    elif type_string in dir(types):
        return getattr(types, type_string)
    else:
        raise NotImplementedError(f"未知的类型: {type_string}")

# 获取泛型参数
def get_generic_args(type_hint: type | Any) -> tuple[type | None, tuple[type, ...] | None]:
    origin = get_origin(type_hint)  # 获取原始类型
    args = get_args(type_hint)      # 获取泛型参数

    if origin is None:
        return None, None
    return origin, args

def is_generic(type_hint: type | Any) -> bool:
    return "__origin__" in dir(type_hint)

def decay_type(type_:type) -> type|List[type]:
    if "__origin__" in dir(type_):
        if "__args__" in dir(type_):
            return list(decay_type(arg) for arg in type_.__args__)
        else:
            return decay_type(type_.__origin__)
    else:
        return type_

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

class BaseInfo(BaseModel, any_class):
    def __init__(self, **kwargs):
        BaseModel.__init__(self, **kwargs)
        any_class.__init__(self)

class MemberInfo(BaseInfo):
    _MemberName:    str             = PrivateAttr(default="")
    _ParentType:    Optional[type]  = PrivateAttr(default=None)
    _IsStatic:      bool            = PrivateAttr(default=False)
    _IsPublic:      bool            = PrivateAttr(default=False)

    def __init__(self, name:str, ctype:type, is_static:bool, is_public:bool, **kwargs):
        super().__init__(**kwargs)
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

    @override
    def __repr__(self) -> str:
        return f"<{self.MemberName}>"
    @override
    def __str__(self) -> str:
        return f"{self.MemberName}"
    @override
    def SymbolName(self) -> str:
        return "MemberInfo"
    @override
    def ToString(self) -> str:
        return f"MemberInfo<name={self.MemberName}, ctype={self.ParentType}, " \
               f"{'static' if self.IsStatic else 'instance'}, {'public' if self.IsPublic else 'private'}>"

class ValueInfo(BaseInfo):
    _RealType:      Optional[type]  = PrivateAttr(default=None)
    _IsPrimitive:   bool            = PrivateAttr(default=False)
    _IsValueType:   bool            = PrivateAttr(default=False)
    _IsCollection:  bool            = PrivateAttr(default=False)
    _IsDictionary:  bool            = PrivateAttr(default=False)
    _IsTuple:       bool            = PrivateAttr(default=False)
    _IsSet:         bool            = PrivateAttr(default=False)
    _IsList:        bool            = PrivateAttr(default=False)
    _IsUnsupported: bool            = PrivateAttr(default=False)

    _IsUnion:       bool            = PrivateAttr(default=False)
    _UnionTypes:    List[type]      = PrivateAttr(default=[])

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

    @property
    def IsUnion(self) -> bool:
        return self._IsUnion
    @property
    def UnionTypes(self) -> List[type]:
        return self._UnionTypes

    def __init__(self, type_:type, **kwargs):
        super().__init__(**kwargs)
        if is_generic(type_) and type_ is not dict and type_ is not list and type_ is not tuple and type_ is not set:
            if GetInternalDebug():
                print_colorful(ConsoleFrontColor.RED, f"Current Original UnionType: {type_}")
            self._UnionTypes = [typeitem for typeitem in decay_type(type_)]
            while any(is_generic(typeItem) for typeItem in self._UnionTypes):
                self._UnionTypes = [typeitem for typeitem in decay_type(type_)]
            self._UnionTypes = [t for t in self._UnionTypes if t is not type(None) or t is not type]
            if GetInternalDebug():
                print_colorful(ConsoleFrontColor.RED, f"Current Final UnionType: {self._UnionTypes}")
            if len(self._UnionTypes) != 1:
                self._IsUnion = True
            else:
                type_ = self._UnionTypes[0]
        if not self._IsUnion:
            self._IsUnion = False
            self._UnionTypes = []
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

    def Verify(self, valueType:type) -> bool:
        if self.IsUnion:
            return any(issubclass(valueType, typeitem) for typeitem in self.UnionTypes)
        else:
            return issubclass(valueType, self.RealType)

    @override
    def __repr__(self) -> str:
        return f"ValueInfo<type={self.RealType if not self.IsUnion else self.UnionTypes}>"
    @override
    def SymbolName(self) -> str:
        return "ValueInfo"
    @override
    def ToString(self) -> str:
        return f"ValueInfo<type={self.RealType if not self.IsUnion else self.UnionTypes}>"

class FieldInfo(MemberInfo, ValueInfo):
    def __init__(self, fieldType:type, name:str, ctype:type, is_static:bool, is_public:bool):
        super().__init__(
            name = name,
            ctype = ctype,
            is_static = is_static,
            is_public = is_public,
            type_ = fieldType,
            )

    @property
    def FieldName(self) -> str:
        '''
        字段名称
        '''
        return self.MemberName
    @property
    def FieldType(self) -> type|List[type]:
        '''
        字段类型
        '''
        if self.IsUnion:
            return self.UnionTypes
        else:
            return self.RealType

    @virtual
    def GetValue(self, obj:Any) -> Any:
        if self.IsStatic:
            return getattr(self.ParentType, self.MemberName)
        else:
            if not isinstance(obj, self.ParentType):
                raise TypeError(f"Parent type mismatch, expected {self.ParentType}, got {type(obj)}")
            return getattr(obj, self.MemberName)
    @virtual
    def SetValue(self, obj:Any, value:Any) -> None:
        if self.IsStatic:
            if self.Verify(type(value)):
                setattr(self.ParentType, self.MemberName, value)
            else:
                raise TypeError(f"Value type mismatch, expected {self.RealType}, got {type(value)}")
        else:
            if not isinstance(obj, self.ParentType):
                raise TypeError(f"Parent type mismatch, expected {self.ParentType}, got {type(obj)}")
            if self.Verify(type(value)):
                setattr(obj, self.MemberName, value)
            else:
                raise TypeError(f"Value type mismatch, expected {self.RealType}, got {type(value)}")

    @override
    def SymbolName(self) -> str:
        return "FieldInfo"
    @override
    def ToString(self) -> str:
        return f"FieldInfo<name={self.MemberName}, type={self.FieldType}, ctype={self.ParentType}, " \
               f"{'static' if self.IsStatic else 'instance'}, {'public' if self.IsPublic else 'private'}>"

class ParameterInfo(ValueInfo):
    _ParameterName: str  = PrivateAttr(default="")
    _IsOptional:    bool = PrivateAttr(default=False)
    _DefaultValue:  Any  = PrivateAttr(default=None)

    def __init__(self, type:type, name:str, is_optional:bool, default_value:Any):
        super().__init__(type)
        self._ParameterName = name
        self._IsOptional = is_optional
        self._DefaultValue = default_value

    @property
    def ParameterName(self) -> str:
        return self._ParameterName
    @property
    def ParameterType(self) -> type|List[type]:
        return self.RealType if not self.IsUnion else self.UnionTypes
    @property
    def IsOptional(self) -> bool:
        return self._IsOptional
    @property
    def DefaultValue(self) -> Any:
        return self._DefaultValue

    @override
    def __repr__(self) -> str:
        return f"<{self.ParameterName}>"
    @override
    def SymbolName(self) -> str:
        return "ParameterInfo"
    @override
    def ToString(self) -> str:
        return f"ParameterInfo<name={self.ParameterName}, type={self.ParameterType}, " \
               f"{'optional' if self.IsOptional else 'required'}, default={self.DefaultValue}>"

class MethodInfo(MemberInfo):
    _ReturnType:            Optional[type]      = PrivateAttr(default=None)
    _Parameters:            List[ParameterInfo] = PrivateAttr(default=[])
    _PositionalParameters:  List[ParameterInfo] = PrivateAttr(default=[])
    _KeywordParameters:     List[ParameterInfo] = PrivateAttr(default=[])
    _IsClassMethod:         bool                = PrivateAttr(default=False)

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
        is_class_method:        bool,
        ):
        MemberInfo.__init__(self, name, ctype, is_static, is_public)
        self._ReturnType = return_type
        self._Parameters = parameters
        self._PositionalParameters = positional_parameters
        self._KeywordParameters = keyword_parameters
        self._IsClassMethod = is_class_method
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
    @property
    def IsClassMethod(self) -> bool:
        return self._IsClassMethod

    @overload
    def Invoke(self, obj:object, *args, **kwargs) -> object:
        '''
        调用实例方法
        '''
        ...
    @overload
    def Invoke(self, obj:type, *args, **kwargs) -> object:
        '''
        调用类方法
        '''
        ...
    @overload
    def Invoke(self, noneObj:Literal[None]|None, *args, **kwargs) -> object:
        '''
        调用静态方法
        '''
        ...
    def Invoke(self, obj:object|type, *args, **kwargs) -> object:
        if not self.IsStatic and obj is None:
            raise TypeError("Object is None")
        if not self.IsStatic and not self.IsClassMethod and not isinstance(obj, self.ParentType):
            raise TypeError(f"Parent type mismatch, expected {self.ParentType}, got {type(obj)}")
        if self.IsClassMethod and not isinstance(obj, type):
            raise TypeError(f"Class method expected type, got {type(obj)}: {obj}")
        result = None
        if self.IsStatic:
            method = getattr(self.ParentType, self.MemberName)
            if method is None:
                raise AttributeError(f"{self.ParentType} type has no method '{self.MemberName}'")
            result = method(*args, **kwargs)
        elif self.IsClassMethod:
            method = getattr(obj, self.MemberName)
            if method is None:
                raise AttributeError(f"{obj} class has no method '{self.MemberName}'")
            result = method(*args, **kwargs)
        else:
            method = getattr(obj, self.MemberName)
            if method is None:
                raise AttributeError(f"{self.ParentType} type has no method '{self.MemberName}'")
            result = method(*args, **kwargs)
        return result
    @override
    def SymbolName(self) -> str:
        return "MethodInfo"
    @override
    def ToString(self) -> str:
        return f"MethodInfo<name={self.MemberName}, return={self.ReturnType}, ctype={self.ParentType}" \
               f"{', static' if self.IsStatic else ''}{', class' if self.IsClassMethod else ''}, {'public' if self.IsPublic else 'private'}, " \
               f"params_count={len(self.Parameters)}>"

class RefTypeFlag(IntFlag):
    Static:int = 0b00000001
    Instance:int = 0b00000010
    Public:int = 0b00000100
    Private:int = 0b00001000
    Default:int = 0b00010000
    Method:int = 0b00100000
    Field:int = 0b01000000
    Special:int = 0b10000000
    All:int = 0b11111111

class RefType(ValueInfo):
    _FieldInfos:    List[FieldInfo]  = PrivateAttr()
    _MethodInfos:   List[MethodInfo] = PrivateAttr()
    _MemberNames:   List[str]        = PrivateAttr()

    def __init__(self, type_:type):
        if is_generic(type_):
            raise NotImplementedError("Generic type is not supported")
        super().__init__(type_)
        self._FieldInfos = []
        self._MethodInfos = []
        self._MemberNames = []

        class_var = type_.__dict__
        annotations:Dict[str, type_] = get_type_hints(type_)

        for name, member in inspect.getmembers(type_):
            if inspect.ismethod(member) or inspect.isfunction(member):
                # 获取方法签名
                sig = inspect.signature(member)
                is_static = isinstance(member, staticmethod)
                is_class_method = isinstance(member, classmethod)
                is_public = (name.startswith("__") and name.endswith("__")) or not name.startswith('_')

                # 构建参数列表
                parameters = []
                positional_parameters = []
                keyword_parameters = []

                for param_name, param in sig.parameters.items():
                    if param_name == 'self':
                        continue
                    if param_name == 'cls':
                        continue
                    ptype = param.annotation if param.annotation != inspect.Parameter.empty else Any
                    ptype = ptype if isinstance(ptype, type) else Any
                    param_info = ParameterInfo(
                        type = ptype,
                        name = param_name,
                        is_optional = param.default != inspect.Parameter.empty,
                        default_value = param.default if param.default != inspect.Parameter.empty else None
                    )
                    parameters.append(param_info)

                    if param.kind == inspect.Parameter.POSITIONAL_ONLY or param.kind == inspect.Parameter.POSITIONAL_OR_KEYWORD:
                        positional_parameters.append(param_info)
                    elif param.kind == inspect.Parameter.KEYWORD_ONLY:
                        keyword_parameters.append(param_info)

                # 构建方法信息
                if GetInternalDebug():
                    print_colorful(ConsoleFrontColor.RED, f"Current Make MethodInfo: {type_}.{name}")
                method_info = MethodInfo(
                    return_type = sig.return_annotation if sig.return_annotation != inspect.Signature.empty else Any,
                    parameters = parameters,
                    positional_parameters = positional_parameters,
                    keyword_parameters = keyword_parameters,
                    name = name,
                    ctype = type_,
                    is_static = is_static,
                    is_public = is_public,
                    is_class_method = is_class_method
                )
                self._MethodInfos.append(method_info)
                self._MemberNames.append(name)

        if issubclass(type_, BaseModel):
            for field_name, model_field in type_.__pydantic_fields__.items():
                #fieldType = decay_type(model_field.annotation) if model_field.annotation is not None else Any
                fieldType = model_field.annotation if model_field.annotation is not None else Any
                if GetInternalDebug():
                    print_colorful(ConsoleFrontColor.RED, f"Current Make FieldInfo: {type_}.{field_name} {fieldType} ")
                field_info = FieldInfo(
                    fieldType=fieldType,
                    name=field_name,
                    ctype=type_,
                    is_public=True,
                    is_static=False,
                )
                self._FieldInfos.append(field_info)
                self._MemberNames.append(field_name)
        else:
            for name, member in inspect.getmembers(type_):
                if not inspect.ismethod(member) and not inspect.isfunction(member):
                    is_static = name in class_var
                    is_public = (name.startswith('__') and name.endswith('__')) or not name.startswith('_')
                    fieldType = annotations.get(name, Any)
                    field_info = FieldInfo(
                        fieldType = fieldType,
                        name = name,
                        ctype = type_,
                        is_static = is_static,
                        is_public = is_public
                    )
                    self._FieldInfos.append(field_info)
                    self._MemberNames.append(name)

        for name, annotation in annotations.items():
            if name not in self._MemberNames:
                field_info = FieldInfo(
                    fieldType = decay_type(annotation),
                    name = name,
                    ctype = type_,
                    is_static = False,
                    is_public = not name.startswith('_')
                )
                self._FieldInfos.append(field_info)
                self._MemberNames.append(name)
    def _where_member(self, member:MemberInfo, flag:RefTypeFlag) -> bool:
        stats = True
        if member.IsStatic:
            stats &= (flag & RefTypeFlag.Static != 0)
        else:
            stats &= (flag & RefTypeFlag.Instance != 0)
        if member.IsPublic:
            stats &= (flag & RefTypeFlag.Public != 0)
        else:
            stats &= (flag & RefTypeFlag.Private != 0)
        if isinstance(member, MethodInfo):
            stats &= (flag & RefTypeFlag.Method != 0)
        elif isinstance(member, FieldInfo):
            stats &= (flag & RefTypeFlag.Field != 0)
        if member.MemberName.startswith('__') and member.MemberName.endswith('__'):
            stats &= (flag & RefTypeFlag.Special != 0)
        return stats

    def GetFields(self, flag:RefTypeFlag=RefTypeFlag.Default) -> List[FieldInfo]:
        if flag == RefTypeFlag.Default:
            return [field for field in self._FieldInfos
                    if self._where_member(field, RefTypeFlag.Field|RefTypeFlag.Public|RefTypeFlag.Instance)]
        else:
            return [field for field in self._FieldInfos if self._where_member(field, flag)]
    def GetAllFields(self) -> List[FieldInfo]:
        return self._FieldInfos
    def GetMethods(self, flag:RefTypeFlag=RefTypeFlag.Default) -> List[MethodInfo]:
        if flag == RefTypeFlag.Default:
            return [method for method in self._MethodInfos
                    if self._where_member(method, RefTypeFlag.Method|RefTypeFlag.Public|RefTypeFlag.Instance|RefTypeFlag.Static)]
        else:
            return [method for method in self._MethodInfos if self._where_member(method, flag)]
    def GetAllMethods(self) -> List[MethodInfo]:
        return self._MethodInfos
    def GetMembers(self, flag:RefTypeFlag=RefTypeFlag.Default) -> List[MemberInfo]:
        if flag == RefTypeFlag.Default:
            return [member for member in self._FieldInfos + self._MethodInfos
                    if self._where_member(member, RefTypeFlag.Public|RefTypeFlag.Instance|RefTypeFlag.Field|RefTypeFlag.Method)]
        else:
            return [member for member in self._FieldInfos + self._MethodInfos if self._where_member(member, flag)]
    def GetAllMembers(self) -> List[MemberInfo]:
        return self._FieldInfos + self._MethodInfos

    def GetField(self, name:str, flags:RefTypeFlag=RefTypeFlag.Default) -> Optional[FieldInfo]:
        return next((field for field in self.GetFields(flags) if field.MemberName == name), None)
    def GetMethod(self, name:str, flags:RefTypeFlag=RefTypeFlag.Default) -> Optional[MethodInfo]:
        return next((method for method in self.GetMethods(flags) if method.MemberName == name), None)
    def GetMember(self, name:str, flags:RefTypeFlag=RefTypeFlag.Default) -> Optional[MemberInfo]:
        return next((member for member in self.GetMembers(flags) if member.MemberName == name), None)

    def GetFieldValue[T](self, obj:object, name:str, flags:RefTypeFlag=RefTypeFlag.Default) -> T:
        field = self.GetField(name, flags)
        if field is not None:
            return field.GetValue(obj)
        else:
            raise ReflectionException(f"Field {name} not found")
    def SetFieldValue[T](self, obj:object, name:str, value:T, flags:RefTypeFlag=RefTypeFlag.Default) -> None:
        field = self.GetField(name, flags)
        if field is not None:
            field.SetValue(obj, value)
        else:
            raise ReflectionException(f"Field {name} not found")
    def InvokeMethod[T](self, obj:object, name:str, flags:RefTypeFlag=RefTypeFlag.Default, *args, **kwargs) -> T:
        method = self.GetMethod(name, flags)
        if method is not None:
            return method.Invoke(obj, *args, **kwargs)
        else:
            raise ReflectionException(f"Method {name} not found")

    def TryGetFieldValue[T](self, obj:object, lv:left_value_reference[T], name:str, flags:RefTypeFlag=RefTypeFlag.Default) -> bool:
        try:
            lv.ref_value = self.GetFieldValue(obj, name, flags)
            return True
        except ReflectionException:
            return False
    def TrySetFieldValue[T](self, obj:object, name:str, value:T, flags:RefTypeFlag=RefTypeFlag.Default) -> bool:
        try:
            self.SetFieldValue(obj, name, value, flags)
            return True
        except ReflectionException:
            return False
    def TryInvokeMethod[T](self, obj:object, lv:left_value_reference[T],
                             name:str, flags:RefTypeFlag=RefTypeFlag.Default, *args, **kwargs) -> bool:
        try:
            lv.ref_value = self.InvokeMethod(obj, name, flags, *args, **kwargs)
            return True
        except ReflectionException:
            return False

    def CreateInstance(self, *args, **kwargs) -> object:
        return self.RealType(*args, **kwargs)

    @override
    def __repr__(self) -> str:
        return f"RefType<type={self.RealType}>"
    @override
    def SymbolName(self) -> str:
        return "RefType"
    @override
    def ToString(self) -> str:
        return f"RefType<type={self.RealType}>"
    def print_str(self, verbose:bool=False, flags:RefTypeFlag=RefTypeFlag.Default) -> str:
        fields:     List[str] = []
        methods:    List[str] = []
        for field in self.GetFields(flags):
            fields.append(f"{ConsoleFrontColor.GREEN if field.IsPublic else ConsoleFrontColor.RED}"\
                f"{field.ToString() if verbose else field.MemberName}{ConsoleFrontColor.RESET}")
        for method in self.GetMethods(flags):
            methods.append(f"{ConsoleFrontColor.YELLOW if method.IsPublic else ConsoleFrontColor.RED}"\
                f"{method.ToString() if verbose else method.MemberName}{ConsoleFrontColor.RESET}")
        return f"RefType<type={self.RealType}{', fields=' if len(fields)!=0 else ''}{', '.join(fields)}{ConsoleFrontColor.RESET}"\
               f"{', methods=' if len(methods)!=0 else ''}{', '.join(methods)}{ConsoleFrontColor.RESET}>"

    @sealed
    def tree(self) -> str:
        type_set: set = set()
        def dfs(currentType:RefType) -> Dict[str, Dict[str, Any]|Any]:
            if currentType.IsPrimitive:
                return currentType.RealType.__name__
            elif currentType.RealType in type_set:
                if currentType.IsUnion:
                    return {
                        "type": currentType.RealType.__name__,
                        "value": { field.FieldName: f"{field.FieldType}" for field in currentType.GetFields() }
                    }
                else:
                    return {
                        "type": currentType.RealType.__name__,
                        "value": { field.FieldName: f"{field.FieldType}" for field in self.GetFields() }
                    }
            else:
                type_set.add(currentType.RealType)
                return {
                    "type": currentType.RealType.__name__,
                    "value": {
                        field.FieldName: dfs(TypeManager.GetInstance().CreateOrGetRefType(field.FieldType))
                        for field in currentType.GetFields()
                        }
                }

        return json.dumps(dfs(self), indent=4)


type RTypen[_T] = RefType
'''
RTypen[T] 是 T 类型的 RefType
'''

_Internal_TypeManager:Optional['TypeManager'] = None

class TypeManager(BaseModel, any_class):
    _RefTypes:Dict[type, RefType] = PrivateAttr(default_factory=dict)

    @classmethod
    def GetInstance(cls) -> Self:
        global _Internal_TypeManager
        if _Internal_TypeManager is None:
            _Internal_TypeManager = cls()
        return _Internal_TypeManager

    @overload
    def CreateOrGetRefType(self, type_:type) -> RefType:
        ...
    @overload
    def CreateOrGetRefType(self, obj:object) -> RefType:
        ...
    @overload
    def CreateOrGetRefType(self, type_str:str) -> RefType:
        ...
    def CreateOrGetRefType(self, type_:type|object|str) -> RefType:
        if isinstance(type_, str):
            type_ = get_type_from_string(type_)
        elif not isinstance(type_, type):
            type_ = type(type_)

        if type_ in self._RefTypes:
            return self._RefTypes[type_]
        else:
            if GetInternalDebug():
                print_colorful(ConsoleFrontColor.RED, f"Current Create RefType: {type_}")
            ref_type = RefType(type_)
            self._RefTypes[type_] = ref_type
            return ref_type

