import                  importlib
import                  inspect
import                  types
from enum        import Enum, IntFlag
from typing      import *
import                  typing
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

_Internal_Reflection_Debug:bool = False
def GetInternalReflectionDebug() -> bool:
    return _Internal_Reflection_Debug and GetInternalDebug()
def SetInternalReflectionDebug(debug:bool) -> None:
    global _Internal_Reflection_Debug
    _Internal_Reflection_Debug = debug

class ReflectionException(Exception):
    def __init__(self, message:str):
        self.message = f"{ConsoleFrontColor.RED}{message}{ConsoleFrontColor.RESET}"
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

def get_type_from_string_with_module(type_string:str, module_name:str) -> type|None:
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
        return None

# 获取泛型参数
def get_generic_args(type_hint: type | Any) -> tuple[type | None, tuple[type, ...] | None]:
    origin = get_origin(type_hint)  # 获取原始类型
    args = get_args(type_hint)      # 获取泛型参数

    if origin is None:
        return None, None
    return origin, args

def is_generic(type_hint: type | Any) -> bool:
    return "__origin__" in dir(type_hint)

class _SpecialIndictaor:
    pass

class ListIndictaor(_SpecialIndictaor):
    elementType:type
    def __init__(self, elementType:type):
        self.elementType = elementType

class DictIndictaor(_SpecialIndictaor):
    keyType:type
    valueType:type
    def __init__(self, keyType:type, valueType:type):
        self.keyType = keyType
        self.valueType = valueType

class TupleIndictaor(_SpecialIndictaor):
    elementTypes:tuple[type, ...]
    def __init__(self, *elementTypes:type):
        self.elementTypes = elementTypes

class SetIndictaor(_SpecialIndictaor):
    elementType:type
    def __init__(self, elementType:type):
        self.elementType = elementType

def to_type(
    typen:          type|Any|str,
    *,
    module_name:    str|None=None
    ) -> type|List[type]|_SpecialIndictaor:
    if isinstance(typen, type):
        return typen
    elif isinstance(typen, _SpecialIndictaor):
        return typen
    elif isinstance(typen, str):
        import sys
        if not all(c.isalnum() or c == '.' for c in typen):
            raise ValueError(f"Invalid type string: {typen}, only alphanumeric characters and dots are allowed")
        type_components = typen.split(".")
        type_module = module_name or (".".join(type_components[:-1]) if len(type_components) > 1 else None)
        type_final = type_components[-1]
        if type_module is not None:
            return sys.modules[type_module].__dict__[type_final]
        else:
            for module in sys.modules.values():
                if type_final in module.__dict__:
                    return module.__dict__[type_final]
            return get_type_from_string(typen)
    elif is_union(typen):
        uTypes = get_union_types(typen)
        uTypes = [uType for uType in uTypes if uType is not type(None)]
        if len(uTypes) == 1:
            return uTypes[0]
        elif len(uTypes) == 0:
            return type(None)
        else:
            return uTypes
    elif hasattr(typen, '__origin__'):
        oType = get_origin(typen)
        if oType is list:
            return ListIndictaor(get_args(typen)[0])
        elif oType is dict:
            return DictIndictaor(get_args(typen)[0], get_args(typen)[1])
        elif oType is tuple:
            return TupleIndictaor(*get_args(typen))
        elif oType is set:
            return SetIndictaor(get_args(typen)[0])
        else:
            return oType
    else:
        return type(typen)

def try_to_type(typen:type|Any|str, *, module_name:str|None=None) -> type|List[type]|_SpecialIndictaor|None:
    try:
        return to_type(typen, module_name=module_name)
    except Exception:
        return None

def is_union(type_hint: type | Any) -> bool:
    return "__origin__" in dir(type_hint) and type_hint.__origin__ == Union

def get_union_types(type_hint: type | Any) -> List[type]:
    return [t for t in type_hint.__args__]

class TypeVarIndictaor:
    pass

class AnyVarIndicator:
    pass

def decay_type(
    type_hint:      type|Any,
    *,
    module_name:    str|None=None
    ) -> type|List[type]|_SpecialIndictaor:
    if GetInternalReflectionDebug():
        print_colorful(ConsoleFrontColor.YELLOW, f"Decay: {type_hint}")
    type_dir = dir(type_hint)
    result:type|List[type] = None
    if isinstance(type_hint, _SpecialIndictaor):
        return type_hint
    elif isinstance(type_hint, str):
        try:
            result = to_type(type_hint, module_name=module_name)
        except TypeError:
            result = Any
    elif "__forward_arg__" in type_dir:
        result = to_type(type_hint.__forward_arg__, module_name=module_name)
    elif type_hint is type:
        result = type_hint
    elif is_union(type_hint):
        result = get_union_types(type_hint)
    elif isinstance(type_hint, TypeVar):
        result = TypeVarIndictaor
    elif type_hint is type:
        result = type_hint
    else:
        raise ReflectionException(f"Invalid type: {type_hint}<{type_hint.__class__}>")
    if GetInternalReflectionDebug():
        print_colorful(ConsoleFrontColor.YELLOW, f"Result: {result}")
    return result

def is_just_defined_in_current_class(member_name:str, current_class:type) -> bool:
    '''
    检查成员是否只在当前类中定义，而不是在父类中定义
    '''
    # 获取当前类的所有成员
    current_members = dict(inspect.getmembers(current_class))
    if member_name not in current_members:
        return False
    # 获取父类的所有成员
    for baseType in current_class.__bases__:
        parent_members = dict(inspect.getmembers(baseType))
        if member_name in parent_members:
            return False
    return True

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
    _RealType:      Optional[Any] = PrivateAttr(default=None)
    _IsPrimitive:   bool          = PrivateAttr(default=False)
    _IsValueType:   bool          = PrivateAttr(default=False)
    _IsCollection:  bool          = PrivateAttr(default=False)
    _IsDictionary:  bool          = PrivateAttr(default=False)
    _IsTuple:       bool          = PrivateAttr(default=False)
    _IsSet:         bool          = PrivateAttr(default=False)
    _IsList:        bool          = PrivateAttr(default=False)
    _IsUnsupported: bool          = PrivateAttr(default=False)
    _GenericArgs:   List[type]    = PrivateAttr(default=[])

    @property
    def IsUnion(self) -> bool:
        return is_union(self._RealType)
    @property
    def RealType(self):
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
    def GenericArgs(self) -> List[type]:
        return self._GenericArgs
    @property
    def Module(self) -> Optional[Dict[str, type]]:
        return sys.modules[self.RealType.__module__]
    @property
    def ModuleName(self) -> str:
        return self.RealType.__module__

    def __init__(self, metaType:type|Any, generic_args:List[type]=[], **kwargs) -> None:
        super().__init__(**kwargs)
        self._RealType = metaType
        if len(generic_args) > 0:
            print_colorful(ConsoleFrontColor.YELLOW, f"Current ValueInfo Debug Frame: "\
                f"metaType={metaType}, generic_args={generic_args}")
        self._GenericArgs = generic_args
        if not isinstance(metaType, type):
            return
        self._IsPrimitive = (
            issubclass(metaType, int) or
            issubclass(metaType, float) or
            issubclass(metaType, str) or
            issubclass(metaType, bool) or
            issubclass(metaType, complex) #or
            # issubclass(metaType, tuple) or
            # issubclass(metaType, set) or
            # issubclass(metaType, list) or
            # issubclass(metaType, dict)
            )
        self._IsValueType = (
            issubclass(metaType, int) or
            issubclass(metaType, float) or
            issubclass(metaType, str) or
            issubclass(metaType, bool) or
            issubclass(metaType, complex)
            )
        self._IsCollection = (
            issubclass(metaType, list) or
            issubclass(metaType, dict) or
            issubclass(metaType, tuple) or
            issubclass(metaType, set)
            )
        self._IsDictionary = (
            issubclass(metaType, dict)
            )
        self._IsTuple = (
            issubclass(metaType, tuple)
            )
        self._IsSet = (
            issubclass(metaType, set)
            )
        self._IsList = (
            issubclass(metaType, list)
            )

    def Verify(self, valueType:type) -> bool:
        if self.IsUnsupported:
            raise ReflectionException(f"Unsupported type: {self.RealType}")
        if self.IsUnion:
            return any(ValueInfo(uType).Verify(valueType) for uType in get_union_types(self.RealType))
        elif self.RealType is Any:
            return True
        elif self.RealType is type(None):
            return valueType is None or valueType is type(None)
        else:
            try:
                return issubclass(valueType, self.RealType)
            except Exception as e:
                raise ReflectionException(f"Verify type {valueType} with {self.RealType}: \n{e}") from e

    def DecayToList(self) -> List[Self]:
        result:List[Self] = []
        if self.IsUnion:
            for uType in get_union_types(self.RealType):
                result.extend(ValueInfo(uType).DecayToList())
        else:
            result.append(self)
        result = list(dict.fromkeys(result).keys())
        return result

    @override
    def __repr__(self) -> str:
        generic_args = ", ".join(self._GenericArgs)
        return f"ValueInfo<{self.RealType}{f'[{generic_args}]' if generic_args else ''}>"
    @override
    def SymbolName(self) -> str:
        return "ValueInfo"
    @override
    def ToString(self) -> str:
        generic_args = ", ".join(self._GenericArgs)
        return f"<{self.RealType}{f'[{generic_args}]' if generic_args else ''}>"

    @staticmethod
    def Create(
        metaType:       type|Any,
        *,
        module_name:    Optional[str] = None,
        SelfType:       type|Any|None        = None,
        **kwargs
        ) -> Self:
        if GetInternalReflectionDebug():
            print_colorful(ConsoleFrontColor.BLUE, f"Current ValueInfo.Create Frame: "\
                f"metaType={metaType}, SelfType={SelfType}")
        if isinstance(metaType, type):
            return ValueInfo(metaType, **kwargs)
        elif isinstance(metaType, str):
            type_ = try_to_type(metaType, module_name=module_name)
            if type_ is None:
                return ValueInfo(metaType, **kwargs)
            else:
                return ValueInfo(type_, **kwargs)
        elif metaType is Self:
            if SelfType is None:
                raise ReflectionException("SelfType is required when metaType is <Self>")
            return ValueInfo.Create(SelfType, **kwargs)
        elif isinstance(metaType, TypeVar):
            return TypeVarIndictaor
        elif hasattr(metaType, '__origin__'):
            oType = get_origin(metaType)
            if oType is list:
                return ValueInfo(list, [get_args(metaType)[0]])
            elif oType is dict:
                return ValueInfo(dict, [get_args(metaType)[0], get_args(metaType)[1]])
            elif oType is tuple:
                return ValueInfo(tuple, to_list(get_args(metaType)))
            elif oType is set:
                return ValueInfo(set, [get_args(metaType)[0]])
        return ValueInfo(metaType, **kwargs)

class FieldInfo(MemberInfo):
    _MetaType:      Optional[ValueInfo] = PrivateAttr(default=None)

    def __init__(
        self,
        metaType:       Any,
        name:           str,
        ctype:          type,
        is_static:      bool,
        is_public:      bool,
        module_name:    Optional[str] = None,
        selfType:       type|Any|None = None
        ):
        if GetInternalReflectionDebug():
            print_colorful(ConsoleFrontColor.LIGHTBLUE_EX, f"Current Make FieldInfo: {ctype}.{name} {metaType} ")
        super().__init__(
            name = name,
            ctype = ctype,
            is_static = is_static,
            is_public = is_public,
            )
        self._MetaType = ValueInfo.Create(metaType, module_name=module_name, SelfType=selfType)
        if GetInternalReflectionDebug():
            print_colorful(ConsoleFrontColor.LIGHTBLUE_EX, f"Current RealType: {self._MetaType.RealType}")

    @property
    def IsUnion(self) -> bool:
        return self._MetaType.IsUnion
    @property
    def FieldName(self) -> str:
        '''
        字段名称
        '''
        return self.MemberName
    @property
    def ValueType(self):
        return self._MetaType
    @property
    def FieldType(self):
        '''
        字段类型
        '''
        return self._MetaType.RealType

    def Verify(self, valueType:type) -> bool:
        return self._MetaType.Verify(valueType)

    @virtual
    def GetValue(self, obj:Any) -> Any:
        if self.IsStatic:
            return getattr(self.ParentType, self.MemberName)
        else:
            if not isinstance(obj, self.ParentType):
                raise TypeError(f"{ConsoleFrontColor.RED}Field {ConsoleFrontColor.LIGHTBLUE_EX}{self.MemberName}"\
                    f"{ConsoleFrontColor.RED} , parent type mismatch, expected {self.ParentType}, got {type(obj)}"\
                    f"{ConsoleFrontColor.RESET}")
            return getattr(obj, self.MemberName)
    @virtual
    def SetValue(self, obj:Any, value:Any) -> None:
        if self.IsStatic:
            if self.Verify(type(value)):
                setattr(self.ParentType, self.MemberName, value)
            else:
                raise TypeError(f"Value type mismatch, expected {self.MetaType.RealType}, got {type(value)}")
        else:
            if not isinstance(obj, self.ParentType):
                raise TypeError(f"Parent type mismatch, expected {self.ParentType}, got {type(obj)}")
            if self.Verify(type(value)):
                setattr(obj, self.MemberName, value)
            else:
                if isinstance(self.FieldType, str):
                    raise TypeError(f"{ConsoleFrontColor.RED}Field {ConsoleFrontColor.LIGHTBLUE_EX}{self.MemberName}"\
                        f"{ConsoleFrontColor.RED} , value type mismatch, expected \"{self.FieldType}\""\
                        f", got {type(value)}{ConsoleFrontColor.RESET}")
                else:
                    raise TypeError(f"{ConsoleFrontColor.RED}Field {ConsoleFrontColor.LIGHTBLUE_EX}{self.MemberName}"\
                        f"{ConsoleFrontColor.RED} , value type mismatch, expected {self.FieldType}"\
                        f", got {type(value)}{ConsoleFrontColor.RESET}")

    @override
    def __repr__(self) -> str:
        return f"<{self.MemberName} type={self.FieldType}>"
    @override
    def SymbolName(self) -> str:
        return "FieldInfo"
    @override
    def ToString(self) -> str:
        return f"FieldInfo<name={self.MemberName}, type={self.FieldType}, ctype={self.ParentType}, " \
               f"{'static' if self.IsStatic else 'instance'}, {'public' if self.IsPublic else 'private'}>"

class ParameterInfo(BaseInfo):
    _MetaType:      Optional[ValueInfo] = PrivateAttr(default=None)
    _ParameterName: str  = PrivateAttr(default="")
    _IsOptional:    bool = PrivateAttr(default=False)
    _DefaultValue:  Any  = PrivateAttr(default=None)

    def __init__(
        self,
        metaType:       Any,
        name:           str,
        is_optional:    bool,
        default_value:  Any,
        module_name:    Optional[str] = None,
        selfType:       type|Any|None = None,
        **kwargs
        ):
        super().__init__(**kwargs)
        self._ParameterName = name
        self._IsOptional = is_optional
        self._DefaultValue = default_value
        self._MetaType = ValueInfo.Create(metaType, module_name=module_name, SelfType=selfType)

    @property
    def ValueType(self):
        return self._MetaType
    @property
    def ParameterName(self) -> str:
        return self._ParameterName
    @property
    def ParameterType(self):
        return self._MetaType.RealType
    @property
    def IsOptional(self) -> bool:
        return self._IsOptional
    @property
    def DefaultValue(self) -> Any:
        return self._DefaultValue

    def Verify(self, valueType:type) -> bool:
        return self._MetaType.Verify(valueType)

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
    _ReturnType:            Optional[ValueInfo] = PrivateAttr(default=None)
    _Parameters:            List[ParameterInfo] = PrivateAttr(default=[])
    _PositionalParameters:  List[ParameterInfo] = PrivateAttr(default=[])
    _KeywordParameters:     List[ParameterInfo] = PrivateAttr(default=[])
    _IsClassMethod:         bool                = PrivateAttr(default=False)

    def __init__(
        self,
        return_type:            Any,
        parameters:             List[ParameterInfo],
        positional_parameters:  List[ParameterInfo],
        keyword_parameters:     List[ParameterInfo],
        name:                   str,
        ctype:                  type,
        is_static:              bool,
        is_public:              bool,
        is_class_method:        bool,
        ):
        if GetInternalReflectionDebug():
            print_colorful(ConsoleFrontColor.YELLOW, f"Current Make MethodInfo: "\
                f"{return_type} {ctype}.{name}({', '.join([p.ParameterName for p in parameters])})")
        MemberInfo.__init__(self, name, ctype, is_static, is_public)
        self._ReturnType = ValueInfo.Create(return_type, SelfType=self.ParentType)
        self._Parameters = parameters
        self._PositionalParameters = positional_parameters
        self._KeywordParameters = keyword_parameters
        self._IsClassMethod = is_class_method
    @property
    def ReturnType(self) -> ValueInfo:
        return self._ReturnType.RealType
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
    _BaseTypes:     List[Self]       = PrivateAttr()

    def __init__(self, metaType:type|_SpecialIndictaor):
        extensionFields:List[FieldInfo] = []
        if isinstance(metaType, ListIndictaor):
            extensionFields.append(FieldInfo(
                metaType = metaType.elementType,
                name = "elementType",
                ctype = metaType,
                is_static = False,
                is_public = True,
                selfType=list
            ))
            metaType = list
        elif isinstance(metaType, DictIndictaor):
            extensionFields.append(FieldInfo(
                metaType = metaType.keyType,
                name = "keyType",
                ctype = metaType,
                is_static = False,
                is_public = True,
                selfType=dict
            ))
            extensionFields.append(FieldInfo(
                metaType = metaType.valueType,
                name = "valueType",
                ctype = metaType,
                is_static = False,
                is_public = True,
                selfType=dict
            ))
            metaType = dict
        elif isinstance(metaType, TupleIndictaor):
            for i, elementType in enumerate(metaType.elementTypes):
                extensionFields.append(FieldInfo(
                    metaType = elementType,
                    name = f"elementType_{i}",
                    ctype = metaType,
                    is_static = False,
                    is_public = True,
                    selfType=tuple
                ))
            metaType = tuple
        elif isinstance(metaType, SetIndictaor):
            extensionFields.append(FieldInfo(
                metaType = metaType.elementType,
                name = "elementType",
                ctype = metaType,
                is_static = False,
                is_public = True,
                selfType=set
            ))
            metaType = set
        elif is_generic(metaType):
            raise NotImplementedError("Generic type is not supported")

        if True:
            super().__init__(metaType)
            self._BaseTypes = []
            for baseType in metaType.__bases__:
                self._BaseTypes.append(TypeManager.GetInstance().CreateOrGetRefType(baseType))

            baseFields = self.GetAllBaseFields()
            baseMethods = self.GetAllBaseMethods()

            self._FieldInfos = extensionFields
            self._MethodInfos = []
            self._MemberNames = []

            class_var = metaType.__dict__
            annotations:Dict[str, metaType] = get_type_hints(metaType)

            for name, member in inspect.getmembers(metaType):
                if (any(name == baseMember.MemberName for baseMember in baseMethods) == False and
                    (inspect.ismethod(member) or inspect.isfunction(member))):
                    # 获取方法签名
                    sig = inspect.signature(member)
                    is_static = isinstance(member, staticmethod)
                    is_class_method = isinstance(member, classmethod)
                    is_public = (name.startswith("__") and name.endswith("__")) or not name.startswith('_')

                    # 构建参数列表
                    parameters:List[ParameterInfo] = []
                    positional_parameters:List[ParameterInfo] = []
                    keyword_parameters:List[ParameterInfo] = []

                    for param_name, param in sig.parameters.items():
                        if param_name == 'self':
                            continue
                        if param_name == 'cls':
                            continue
                        ptype = param.annotation if param.annotation != inspect.Parameter.empty else Any
                        ptype = ptype if isinstance(ptype, type) else Any
                        param_info = ParameterInfo(
                            metaType = ptype,
                            name = param_name,
                            is_optional = param.default != inspect.Parameter.empty,
                            default_value = param.default if param.default != inspect.Parameter.empty else None,
                            module_name = self.ModuleName,
                            selfType=metaType
                        )
                        parameters.append(param_info)

                        if param.kind == inspect.Parameter.POSITIONAL_ONLY or param.kind == inspect.Parameter.POSITIONAL_OR_KEYWORD:
                            positional_parameters.append(param_info)
                        elif param.kind == inspect.Parameter.KEYWORD_ONLY:
                            keyword_parameters.append(param_info)

                    # 构建方法信息
                    method_info = MethodInfo(
                        return_type = sig.return_annotation if sig.return_annotation != inspect.Signature.empty else Any,
                        parameters = parameters,
                        positional_parameters = positional_parameters,
                        keyword_parameters = keyword_parameters,
                        name = name,
                        ctype = metaType,
                        is_static = is_static,
                        is_public = is_public,
                        is_class_method = is_class_method
                    )
                    self._MethodInfos.append(method_info)
                    self._MemberNames.append(name)

            if issubclass(metaType, BaseModel):
                try:
                    fields = metaType.model_fields
                except AttributeError:
                    fields = metaType.__pydantic_fields__
                for field_name, model_field in fields.items():
                    if any(field_name == baseField.MemberName for baseField in baseFields) == False:
                        fieldType = model_field.annotation if model_field.annotation is not None else Any
                        is_public = not model_field.exclude
                        field_info = FieldInfo(
                            metaType=fieldType,
                            name=field_name,
                            ctype=metaType,
                            is_public=is_public,
                            is_static=False,
                            module_name = self.ModuleName,
                            selfType=metaType
                        )
                        self._FieldInfos.append(field_info)
                        self._MemberNames.append(field_name)
            else:
                for name, member in inspect.getmembers(metaType):
                    if (any(name == baseMember.MemberName for baseMember in baseMethods) == False and
                        not inspect.ismethod(member) and not inspect.isfunction(member)):
                        is_static = name in class_var
                        is_public = (name.startswith('__') and name.endswith('__')) or not name.startswith('_')
                        fieldType = annotations.get(name, Any)
                        field_info = FieldInfo(
                            metaType = fieldType,
                            name = name,
                            ctype = metaType,
                            is_static = is_static,
                            is_public = is_public,
                            module_name = self.ModuleName,
                            selfType=metaType
                        )
                        self._FieldInfos.append(field_info)
                        self._MemberNames.append(name)

            for name, annotation in annotations.items():
                if (any(name == baseMember.MemberName for baseMember in baseFields) == False and
                    name not in self._MemberNames):
                    field_info = FieldInfo(
                        metaType = decay_type(annotation),
                        name = name,
                        ctype = metaType,
                        is_static = False,
                        is_public = not name.startswith('_'),
                        module_name = self.ModuleName,
                        selfType=metaType
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

    def GetBaseFields(self, flag:RefTypeFlag=RefTypeFlag.Default) -> List[FieldInfo]:
        result = []
        for baseType in self._BaseTypes:
            result.extend(baseType.GetFields(flag))
        return result
    def GetAllBaseFields(self) -> List[FieldInfo]:
        result = []
        for baseType in self._BaseTypes:
            result.extend(baseType.GetAllFields())
        return result
    def GetBaseMethods(self, flag:RefTypeFlag=RefTypeFlag.Default) -> List[MethodInfo]:
        result = []
        for baseType in self._BaseTypes:
            result.extend(baseType.GetMethods(flag))
        return result
    def GetAllBaseMethods(self) -> List[MethodInfo]:
        result = []
        for baseType in self._BaseTypes:
            result.extend(baseType.GetAllMethods())
        return result
    def GetBaseMembers(self, flag:RefTypeFlag=RefTypeFlag.Default) -> List[MemberInfo]:
        result = []
        for baseType in self._BaseTypes:
            result.extend(baseType.GetMembers(flag))
        return result
    def GetAllBaseMembers(self) -> List[MemberInfo]:
        result = []
        for baseType in self._BaseTypes:
            result.extend(baseType.GetAllMembers())
        return result

    def GetFields(self, flag:RefTypeFlag=RefTypeFlag.Default) -> List[FieldInfo]:
        if flag == RefTypeFlag.Default:
            result = [field for field in self._FieldInfos
                    if self._where_member(field, RefTypeFlag.Field|RefTypeFlag.Public|RefTypeFlag.Instance)]
        else:
            result = [field for field in self._FieldInfos if self._where_member(field, flag)]
        result.extend(self.GetBaseFields(flag))
        return result
    def GetAllFields(self) -> List[FieldInfo]:
        result = self._FieldInfos
        result.extend(self.GetAllBaseFields())
        return result
    def GetMethods(self, flag:RefTypeFlag=RefTypeFlag.Default) -> List[MethodInfo]:
        if flag == RefTypeFlag.Default:
            result = [method for method in self._MethodInfos
                    if self._where_member(method, RefTypeFlag.Method|RefTypeFlag.Public|RefTypeFlag.Instance|RefTypeFlag.Static)]
        else:
            result = [method for method in self._MethodInfos if self._where_member(method, flag)]
        result.extend(self.GetBaseMethods(flag))
        return result
    def GetAllMethods(self) -> List[MethodInfo]:
        result = self._MethodInfos
        result.extend(self.GetAllBaseMethods())
        return result
    def GetMembers(self, flag:RefTypeFlag=RefTypeFlag.Default) -> List[MemberInfo]:
        if flag == RefTypeFlag.Default:
            result = [member for member in self._FieldInfos + self._MethodInfos
                    if self._where_member(member, RefTypeFlag.Public|RefTypeFlag.Instance|RefTypeFlag.Field|RefTypeFlag.Method)]
        else:
            result = [member for member in self._FieldInfos + self._MethodInfos if self._where_member(member, flag)]
        result.extend(self.GetBaseMembers(flag))
        return result
    def GetAllMembers(self) -> List[MemberInfo]:
        result = self._FieldInfos + self._MethodInfos
        result.extend(self.GetAllBaseMembers())
        return result

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
        return f"RefType<{self.RealType}>"
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
    def tree(self, indent:int=4) -> str:
        type_set: set = set()
        def dfs(currentType:RefType) -> Dict[str, Dict[str, Any]|Any]:
            if GetInternalReflectionDebug():
                print_colorful(ConsoleFrontColor.RED, f"Current Tree DFS: "\
                    f"__type={currentType.RealType} __type.class={currentType.RealType.__class__}")
            if currentType.IsPrimitive:
                return f"{currentType.RealType}"
            elif currentType.RealType in type_set:
                return {
                    "type": f"{currentType.RealType}",
                    "value": { field.FieldName: f"{field.FieldType}" for field in currentType.GetFields() }
                }
            else:
                type_set.add(currentType.RealType)
                value = {}
                for field in currentType.GetFields():
                    value[field.FieldName] = dfs(TypeManager.GetInstance().CreateOrGetRefType(field.FieldType))
                return {
                    "type": f"{currentType.RealType}",
                    "value": value
                }

        return json.dumps(dfs(self), indent=indent)

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

    def AllRefTypes(self) -> Tuple[RefType, ...]:
        return tuple(self._RefTypes.values())

    @staticmethod
    def _TurnToType(data:Any, module_name:Optional[str]=None) -> type|_SpecialIndictaor:
        metaType:type|_SpecialIndictaor = None
        if isinstance(data, str):
            if module_name is None:
                metaType = sys.modules[module_name][data]
        if metaType is None:
            metaType = try_to_type(data, module_name=module_name)
            if metaType is None or isinstance(metaType, list):
                metaType = data
        return metaType

    @overload
    def CreateOrGetRefType(
        self,
        type_:          type,
        module_name:    Optional[str] = None
        ) -> RefType:
        ...
    @overload
    def CreateOrGetRefType(
        self,
        obj:            object,
        module_name:    Optional[str] = None
        ) -> RefType:
        ...
    @overload
    def CreateOrGetRefType(
        self,
        type_str:       str,
        module_name:    Optional[str] = None
        ) -> RefType:
        ...
    def CreateOrGetRefType(
        self,
        data,
        module_name:    Optional[str] = None
        ) -> RefType:
        if data is None:
            raise ReflectionException("data is None")

        metaType:type = TypeManager._TurnToType(data, module_name=module_name)

        if metaType in self._RefTypes:
            if GetInternalReflectionDebug():
                print_colorful(ConsoleFrontColor.GREEN, f"Get Existing RefType: {metaType}")
            return self._RefTypes[metaType]
        else:
            return self.CreateRefType(metaType, module_name=module_name)

    @overload
    def CreateRefType(
        self,
        type_:          type,
        module_name:    Optional[str] = None
        ) -> RefType:
        ...
    @overload
    def CreateRefType(
        self,
        obj:            object,
        module_name:    Optional[str] = None
        ) -> RefType:
        ...
    @overload
    def CreateRefType(
        self,
        type_str:       str,
        module_name:    Optional[str] = None
        ) -> RefType:
        ...
    def CreateRefType(
        self,
        data,
        module_name:    Optional[str] = None
        ) -> RefType:
        if data is None:
            raise ReflectionException("data is None")

        metaType:type|_SpecialIndictaor = TypeManager._TurnToType(data, module_name=module_name)

        if GetInternalReflectionDebug():
            print_colorful(ConsoleFrontColor.GREEN, f"Create RefType: {metaType}")
        try:
            ref_type = RefType(metaType)
            self._RefTypes[metaType] = ref_type
            return ref_type
        except Exception as e:
            raise ReflectionException(f"Create RefType failed: {e}")

    def CreateOrGetRefTypeFromType(self, type_:type) -> RefType:
        if type_ in self._RefTypes:
            return self._RefTypes[type_]
        else:
            return self.CreateRefType(type_)
    def CreateRefTypeFromType(self, type_:type) -> RefType:
        result = self._RefTypes[type_] = self.CreateRefType(type_)
        return result


