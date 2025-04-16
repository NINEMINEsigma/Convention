import                  importlib
import                  inspect
import                  types
import                  weakref
from enum        import Enum, IntFlag
from typing      import *
import                  typing
from ..Internal  import *
from pydantic    import BaseModel, Field, PrivateAttr
import                  json
import functools
import concurrent.futures
from typing import Set

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

# 缓存get_type_from_string的结果
_type_string_cache: Dict[str, type] = {}

class ReflectionException(Exception):
    def __init__(self, message:str):
        self.message = f"{ConsoleFrontColor.RED}{message}{ConsoleFrontColor.RESET}"
        super().__init__(self.message)

def get_type_from_string(type_string:str) -> type:
        """
        根据字符串生成类型，使用缓存提高性能
        """
        # 检查缓存
        if type_string in _type_string_cache:
            return _type_string_cache[type_string]

        result = None

        # 检查内置类型映射
        if type_string in type_symbols:
            result = type_symbols[type_string]
        # 从内置类型中获取
        elif type_string in dir(types):
            result = getattr(types, type_string)
        # 从当前全局命名空间获取
        elif type_string in globals():
            result = globals().get(type_string)
        # 从当前模块获取
        elif type_string in dir(__import__(__name__)):
            result = getattr(__import__(__name__), type_string)
        # 尝试从其他模块获取
        else:
            try:
                if '.' not in type_string:
                    raise ValueError(f"Empty module name, type_string is {type_string}")
                module_name, _, class_name = type_string.rpartition('.')
                if not module_name:
                    raise ValueError(f"Empty module name, type_string is {type_string}")

                # 首先尝试直接获取模块
                try:
                    module = sys.modules[module_name]
                except KeyError:
                    # 模块未加载，需要导入
                    module = importlib.import_module(module_name)

                result = getattr(module, class_name)
            except (ImportError, AttributeError, ValueError) as ex:
                raise TypeError(f"Cannot find type '{type_string}', type_string is <{type_string}>") from ex

        # 更新缓存
        if result is not None:
            _type_string_cache[type_string] = result

        return result

@functools.lru_cache(maxsize=256)
def get_type_from_string_with_module(type_string:str, module_name:str) -> type|None:
    '''
    根据字符串生成类型，带模块名参数，使用缓存
    '''
    # 检查内置类型映射
    if type_string in type_symbols:
        return type_symbols[type_string]

    # 尝试从指定模块获取
    try:
        module = sys.modules.get(module_name)
        if module and type_string in dir(module):
            return getattr(module, type_string)
    except (KeyError, AttributeError):
        pass

    # 尝试从类型模块获取
    if type_string in dir(types):
        return getattr(types, type_string)

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

    def __repr__(self) -> str:
        return f"ListIndictaor<elementType={self.elementType}>"
    def __str__(self) -> str:
        return self.__repr__()
    @override
    def ToString(self) -> str:
        return self.__repr__()

    def __hash__(self) -> int:
        return hash(List[self.elementType])

class DictIndictaor(_SpecialIndictaor):
    keyType:type
    valueType:type
    def __init__(self, keyType:type, valueType:type):
        self.keyType = keyType
        self.valueType = valueType

    def __repr__(self) -> str:
        return f"DictIndictaor<keyType={self.keyType}, valueType={self.valueType}>"
    def __str__(self) -> str:
        return self.__repr__()
    @override
    def ToString(self) -> str:
        return self.__repr__()

    def __hash__(self) -> int:
        return hash(Dict[self.keyType, self.valueType])

class TupleIndictaor(_SpecialIndictaor):
    elementTypes:Tuple[type, ...]
    def __init__(self, *elementTypes:type):
        self.elementTypes = elementTypes

    def __repr__(self) -> str:
        return f"TupleIndictaor<{', '.join(map(str, self.elementTypes))}>"
    def __str__(self) -> str:
        return self.__repr__()
    @override
    def ToString(self) -> str:
        return self.__repr__()

    def __hash__(self) -> int:
        return hash(Tuple[self.elementTypes])

class SetIndictaor(_SpecialIndictaor):
    elementType:type
    def __init__(self, elementType:type):
        self.elementType = elementType

    def __repr__(self) -> str:
        return f"SetIndictaor<elementType={self.elementType}>"
    def __str__(self) -> str:
        return self.__repr__()
    @override
    def ToString(self) -> str:
        return self.__repr__()

    def __hash__(self) -> int:
        return hash(Set[self.elementType])


# 添加记忆化装饰器
def memoize(func):
    cache = {}
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        key = str(args) + str(kwargs)
        if key not in cache:
            cache[key] = func(*args, **kwargs)
        return cache[key]
    return wrapper

# 优化to_type函数
@memoize
def to_type(
    typen:          type|Any|str,
    *,
    module_name:    str|None=None
    ) -> type|List[type]|_SpecialIndictaor:
    # 快速路径：如果已经是类型，直接返回
    if isinstance(typen, type):
        return typen
    elif isinstance(typen, _SpecialIndictaor):
        return typen
    elif isinstance(typen, str):
        # 快速路径：检查字符串的格式
        if '.' in typen:
            # 可能是带模块名的类型
            try:
                module_parts = typen.split('.')
                # 尝试从sys.modules中获取模块
                current_module = sys.modules
                for part in module_parts[:-1]:
                    if part in current_module:
                        current_module = current_module[part]
                    else:
                        # 需要导入模块
                        current_module = importlib.import_module('.'.join(module_parts[:-1]))
                        break

                # 获取类型
                if isinstance(current_module, dict):
                    # 从字典中获取
                    result = current_module.get(module_parts[-1])
                    if isinstance(result, type):
                        return result
                else:
                    # 从模块对象中获取
                    result = getattr(current_module, module_parts[-1], None)
                    if isinstance(result, type):
                        return result
            except (ImportError, AttributeError):
                pass

        # 回退到一般处理
        import sys
        if not all(c.isalnum() or c == '.' for c in typen):
            raise ValueError(f"Invalid type string: {typen}, only alphanumeric characters and dots are allowed")
        type_components = typen.split(".")
        type_module = module_name or (".".join(type_components[:-1]) if len(type_components) > 1 else None)
        type_final = type_components[-1]
        if GetInternalReflectionDebug():
            print_colorful(ConsoleFrontColor.YELLOW, f"type_module: {type_module}, type_final: {type_final}, "\
                f"typen: {typen}, type_components: {type_components}")
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

# 优化decay_type函数
@memoize
def decay_type(
    type_hint:      type|Any,
    *,
    module_name:    str|None=None
    ) -> type|List[type]|_SpecialIndictaor:
    # 快速路径：直接判断常见类型
    if isinstance(type_hint, (type, _SpecialIndictaor)):
        return type_hint

    if GetInternalReflectionDebug():
        print_colorful(ConsoleFrontColor.YELLOW, f"Decay: {type_hint}")

    result:type|List[type] = None

    # 处理字符串类型
    if isinstance(type_hint, str):
        try:
            result = to_type(type_hint, module_name=module_name)
        except TypeError:
            result = Any
    # 处理forward reference
    elif hasattr(type_hint, "__forward_arg__"):
        result = to_type(type_hint.__forward_arg__, module_name=module_name)
    # 处理type类型
    elif type_hint is type:
        result = type_hint
    # 处理union类型
    elif is_union(type_hint):
        result = get_union_types(type_hint)
    # 处理TypeVar
    elif isinstance(type_hint, TypeVar):
        result = TypeVarIndictaor
    # 处理泛型类型
    elif is_generic(type_hint):
        result = get_origin(type_hint)
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
        
    @virtual
    def __str__(self) -> str:
        return self.ToString()

class MemberInfo(BaseInfo):
    _MemberName:    str             = PrivateAttr(default="")
    _ParentType:    Optional[type]  = PrivateAttr(default=None)
    _IsStatic:      bool            = PrivateAttr(default=False)
    _IsPublic:      bool            = PrivateAttr(default=False)

    def __init__(self, name:str, ctype:Optional[type], is_static:bool, is_public:bool, **kwargs):
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
    def IsGeneric(self) -> bool:
        return len(self._GenericArgs) > 0
    @property
    def Module(self) -> Optional[Dict[str, type]]:
        return sys.modules[self.RealType.__module__]
    @property
    def ModuleName(self) -> str:
        return self.RealType.__module__

    def __init__(self, metaType:type|Any, generic_args:List[type]=[], **kwargs) -> None:
        super().__init__(**kwargs)
        self._RealType = metaType
        if GetInternalReflectionDebug() and len(generic_args) > 0:
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
            if metaType is list:
                return ValueInfo(list, [Any])
            elif metaType is dict:
                return ValueInfo(dict, [Any, Any])
            elif metaType is tuple:
                return ValueInfo(tuple, [])
            elif metaType is set:
                return ValueInfo(set, [Any])
            else:
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
            gargs = get_generic_args(metaType)
            if len(gargs) == 1:
                return ValueInfo(gargs[0], **kwargs)
            else:
                return ValueInfo(Any, **kwargs)
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
            print_colorful(ConsoleFrontColor.LIGHTBLUE_EX, f"Current Make FieldInfo: {ctype}."\
                f"{ConsoleFrontColor.RESET}{name} {ConsoleFrontColor.LIGHTBLUE_EX}{metaType} ")
        super().__init__(
            name = name,
            ctype = ctype,
            is_static = is_static,
            is_public = is_public,
            )
        self._MetaType = ValueInfo.Create(metaType, module_name=module_name, SelfType=selfType)
        if GetInternalReflectionDebug():
            print_colorful(ConsoleFrontColor.LIGHTBLUE_EX, f"Current RealType: {self.FieldType}"\
                f"{f'<{self.ValueType.GenericArgs}>' if self.ValueType.IsGeneric else ''}")

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
        return f"FieldInfo<name={self.MemberName}, type={self.FieldType}, ctype={self.ParentType}, "\
            f"{("generics="+str(self.ValueType.GenericArgs)+", ") if self.ValueType.IsGeneric else ''}" \
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

    @classmethod
    def Create(
        cls,
        name:           str,
        method:         Callable,
        ctype:          Optional[type]  = None,
        module_name:    Optional[str]   = None
        ) -> Self:
        '''
        创建MethodInfo对象
        name: 方法名
        method: 方法对象
        ctype: 方法所属的类
        module_name: 模块名
        '''
        # 获取方法签名
        sig = inspect.signature(method)
        is_static = isinstance(method, staticmethod)
        is_class_method = isinstance(method, classmethod)
        is_public = (name.startswith("__") and name.endswith("__")) or not name.startswith('_')
        # 构建参数列表
        parameters:List[ParameterInfo] = []
        positional_parameters:List[ParameterInfo] = []
        keyword_parameters:List[ParameterInfo] = []
        for param_name, param in sig.parameters.items():
            if param_name in ('self', 'cls'):
                continue
            ptype = param.annotation if param.annotation != inspect.Parameter.empty else Any
            ptype = ptype if isinstance(ptype, type) else Any
            param_info = ParameterInfo(
                metaType = ptype,
                name = param_name,
                is_optional = param.default != inspect.Parameter.empty,
                default_value = param.default if param.default != inspect.Parameter.empty else None,
                module_name = module_name,
                selfType=ctype
            )
            parameters.append(param_info)
            if param.kind in (inspect.Parameter.POSITIONAL_ONLY, inspect.Parameter.POSITIONAL_OR_KEYWORD):
                positional_parameters.append(param_info)
            elif param.kind == inspect.Parameter.KEYWORD_ONLY:
                keyword_parameters.append(param_info)
        # 构建方法信息
        return MethodInfo(
            return_type = sig.return_annotation if sig.return_annotation != inspect.Signature.empty else Any,
            parameters = parameters,
            positional_parameters = positional_parameters,
            keyword_parameters = keyword_parameters,
            name = name,
            ctype = ctype,
            is_static = is_static,
            is_public = is_public,
            is_class_method = is_class_method
        )

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

_RefTypeLock:Dict[Any|type|_SpecialIndictaor, threading.Lock] = {}

class RefType(ValueInfo):
    _FieldInfos:    List[FieldInfo]  = PrivateAttr()
    _MethodInfos:   List[MethodInfo] = PrivateAttr()
    _MemberNames:   List[str]        = PrivateAttr()
    _BaseTypes:     List[Self]       = PrivateAttr(default=None)
    _initialized:   bool             = PrivateAttr(default=False)
    _BaseMemberNamesSet: Set[str]    = PrivateAttr(default_factory=set)
    _member_cache:  Dict[Tuple[str, RefTypeFlag], Optional[MemberInfo]] = PrivateAttr(default_factory=dict)

    def __init__(self, metaType:type|_SpecialIndictaor):
        if isinstance(metaType, ListIndictaor):
            super().__init__(list, generic_args=[metaType.elementType])
            metaType = list
        elif isinstance(metaType, DictIndictaor):
            super().__init__(dict, generic_args=[metaType.keyType, metaType.valueType])
            metaType = dict
        elif isinstance(metaType, TupleIndictaor):
            super().__init__(tuple, generic_args=metaType.elementTypes)
            metaType = tuple
        elif isinstance(metaType, SetIndictaor):
            super().__init__(set, generic_args=[metaType.elementType])
            metaType = set
        elif is_generic(metaType):
            raise NotImplementedError("Generic type is not supported")
        else:
            super().__init__(metaType)

        self._FieldInfos = []
        self._MethodInfos = []
        self._MemberNames = []
        self._BaseMemberNamesSet = set()
        self._member_cache = {}

        # 延迟初始化标志
        self._initialized = False

    def _ensure_initialized(self):
        """确保完全初始化，实现延迟加载"""

        # 初始化锁, 防止同个RefType在多线程中被重复初始化
        if self.RealType not in _RefTypeLock:
            _RefTypeLock[self.RealType] = threading.Lock()
        lock_guard(_RefTypeLock[self.RealType])

        if self._initialized:
            return

        metaType = self.RealType
        # 初始化基类列表 - 只初始化一次
        if self._BaseTypes is None:
            self._BaseTypes = []
            for baseType in metaType.__bases__:
                if baseType == metaType:  # 避免循环引用
                    continue
                self._BaseTypes.append(TypeManager.GetInstance().CreateOrGetRefType(baseType))

        # 如果BaseMemberNamesSet为空，则填充它
        if not self._BaseMemberNamesSet:
            # 使用更高效的方式收集基类成员名称
            for baseType in self._BaseTypes:
                # 确保基类已初始化
                baseType._ensure_initialized()
                # 直接合并集合，而不是逐个添加
                self._BaseMemberNamesSet.update(baseType._MemberNames)
                self._BaseMemberNamesSet.update(baseType._BaseMemberNamesSet)

        class_var = metaType.__dict__
        # 只在需要时获取类型注解
        annotations_dict = {}
        try:
            annotations_dict = get_type_hints(metaType)
        except (TypeError, NameError):
            # 类型提示获取失败时，使用空字典
            pass

        # 减少函数调用开销的辅助函数，改为内联处理
        methods_info = []
        method_names = []
        fields_info = self._FieldInfos.copy()  # 保留extensionFields
        field_names = []

        # 一次性收集所有成员，避免多次遍历
        members = inspect.getmembers(metaType)

        # 处理方法
        for name, member in members:
            if name not in self._BaseMemberNamesSet:
                if inspect.ismethod(member) or inspect.isfunction(member):
                    methods_info.append(MethodInfo.Create(name, member, ctype=metaType, module_name=self.ModuleName))
                    method_names.append(name)
                # 处理字段 (非方法成员)
                elif not name.startswith('__'):  # 排除魔术方法相关的属性
                    is_static = name in class_var
                    is_public = (name.startswith('__') and name.endswith('__')) or not name.startswith('_')
                    fieldType = annotations_dict.get(name, Any)
                    field_info = FieldInfo(
                        metaType = fieldType,
                        name = name,
                        ctype = metaType,
                        is_static = is_static,
                        is_public = is_public,
                        module_name = self.ModuleName,
                        selfType=metaType
                    )
                    fields_info.append(field_info)
                    field_names.append(name)

        # 处理BaseModel字段 - 这些通常有特殊的处理
        if issubclass(metaType, BaseModel):
            try:
                fields = getattr(metaType, 'model_fields', getattr(metaType, '__pydantic_fields__', {}))
                for field_name, model_field in fields.items():
                    if field_name not in self._BaseMemberNamesSet and field_name not in field_names:
                        fieldType = model_field.annotation if model_field.annotation is not None else Any
                        is_public = not getattr(model_field, 'exclude', False)
                        field_info = FieldInfo(
                            metaType=fieldType,
                            name=field_name,
                            ctype=metaType,
                            is_public=is_public,
                            is_static=False,
                            module_name=self.ModuleName,
                            selfType=metaType
                        )
                        fields_info.append(field_info)
                        field_names.append(field_name)
            except (AttributeError, TypeError):
                pass  # 忽略BaseModel相关错误

        # 处理注释中的字段 - 只处理尚未添加的字段
        for name, annotation in annotations_dict.items():
            if name not in self._BaseMemberNamesSet and name not in field_names and not name.startswith('__'):
                field_info = FieldInfo(
                    metaType=decay_type(annotation),
                    name=name,
                    ctype=metaType,
                    is_static=False,
                    is_public=not name.startswith('_'),
                    module_name=self.ModuleName,
                    selfType=metaType
                )
                fields_info.append(field_info)
                field_names.append(name)

        # 更新成员列表
        self._MethodInfos = methods_info
        self._FieldInfos = fields_info
        self._MemberNames = method_names + field_names

        self._initialized = True

    def _where_member(self, member:MemberInfo, flag:RefTypeFlag) -> bool:
        # 如果是默认标志，直接根据常见条件判断，避免位运算
        if flag == RefTypeFlag.Default:
            if isinstance(member, MethodInfo):
                return member.IsPublic and (member.IsInstance or member.IsStatic)
            elif isinstance(member, FieldInfo):
                return member.IsPublic and member.IsInstance
            return False

        # 否则进行完整的标志检查
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

    # 修改获取成员方法，使用缓存
    def GetField(self, name:str, flags:RefTypeFlag=RefTypeFlag.Default) -> Optional[FieldInfo]:
        cache_key = (name, flags)
        if cache_key in self._member_cache:
            return self._member_cache[cache_key]

        result = next((field for field in self.GetFields(flags) if field.MemberName == name), None)
        self._member_cache[cache_key] = result
        return result

    def GetMethod(self, name:str, flags:RefTypeFlag=RefTypeFlag.Default) -> Optional[MethodInfo]:
        cache_key = (name, flags)
        if cache_key in self._member_cache:
            return self._member_cache[cache_key]

        result = next((method for method in self.GetMethods(flags) if method.MemberName == name), None)
        self._member_cache[cache_key] = result
        return result

    def GetMember(self, name:str, flags:RefTypeFlag=RefTypeFlag.Default) -> Optional[MemberInfo]:
        cache_key = (name, flags)
        if cache_key in self._member_cache:
            return self._member_cache[cache_key]

        result = next((member for member in self.GetMembers(flags) if member.MemberName == name), None)
        self._member_cache[cache_key] = result
        return result

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
        return f"RefType<{self.RealType}{f'<{self.GenericArgs}>' if self.IsGeneric else ''}>"
    @override
    def SymbolName(self) -> str:
        return "RefType"
    @override
    def ToString(self) -> str:
        return f"RefType<type={self.RealType}, generic={self.GenericArgs}>"
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
            if currentType.IsPrimitive:
                if GetInternalReflectionDebug():
                    print_colorful(ConsoleFrontColor.RED, f"Current Tree DFS(IsPrimitive): "\
                        f"__type={currentType.RealType} __type.class={currentType.RealType.__class__}")
                return f"{currentType.RealType}"
            elif currentType.RealType in type_set:
                if GetInternalReflectionDebug():
                    print_colorful(ConsoleFrontColor.RED, f"Current Tree DFS(Already): "\
                        f"__type={currentType.RealType} __type.class={currentType.RealType.__class__}")
                return {
                    "type": f"{currentType.RealType}",
                    "value": { field.FieldName: f"{field.FieldType}" for field in currentType.GetFields() }
                }
            else:
                if GetInternalReflectionDebug():
                    print_colorful(ConsoleFrontColor.RED, f"Current Tree DFS(New): "\
                        f"__type={currentType.RealType} __type.class={currentType.RealType.__class__}")
                type_set.add(currentType.RealType)
                value = {}
                fields = currentType.GetFields()
                if GetInternalReflectionDebug():
                    print_colorful(ConsoleFrontColor.RED, f"Current Tree DFS(Fields): {[field.FieldName for field in fields]}")
                for field in fields:
                    value[field.FieldName] = dfs(TypeManager.GetInstance().CreateOrGetRefType(field.FieldType))
                return {
                    "type": f"{currentType.RealType}",
                    "value": value
                }

        return json.dumps(dfs(self), indent=indent)

    @override
    def __hash__(self) -> int:
        """使RefType对象可哈希，基于RealType和GenericArgs的哈希值"""
        return hash((self.RealType, tuple(self.GenericArgs)))

    @override
    def __eq__(self, other) -> bool:
        """比较两个RefType对象是否相等，基于RealType的比较"""
        if not isinstance(other, RefType):
            return False
        return self.RealType == other.RealType

    # 添加新的优化方法，避免重复检查
    def _init_base_types_if_needed(self):
        """初始化基类类型，只在需要时执行"""
        if self._BaseTypes is None:
            self._BaseTypes = []
            metaType = self.RealType
            for baseType in metaType.__bases__:
                # 避免循环引用，如果baseType是自己，则跳过
                if baseType == metaType:
                    continue
                self._BaseTypes.append(TypeManager.GetInstance().CreateOrGetRefType(baseType))

    # 确保正确地实现所有GetBase*方法
    @functools.lru_cache(maxsize=128)
    def GetBaseFields(self, flag:RefTypeFlag=RefTypeFlag.Default) -> List[FieldInfo]:
        if self._BaseTypes is None:
            self._init_base_types_if_needed()
        result = []
        for baseType in self._BaseTypes:
            result.extend(baseType.GetFields(flag))
        return result

    @functools.lru_cache(maxsize=128)
    def GetAllBaseFields(self) -> List[FieldInfo]:
        if self._BaseTypes is None:
            self._init_base_types_if_needed()
        result = []
        for baseType in self._BaseTypes:
            result.extend(baseType.GetAllFields())
        return result

    # 修改所有的GetBase*方法
    @functools.lru_cache(maxsize=128)
    def GetBaseMethods(self, flag:RefTypeFlag=RefTypeFlag.Default) -> List[MethodInfo]:
        if self._BaseTypes is None:
            self._init_base_types_if_needed()
        result = []
        for baseType in self._BaseTypes:
            result.extend(baseType.GetMethods(flag))
        return result

    @functools.lru_cache(maxsize=128)
    def GetAllBaseMethods(self) -> List[MethodInfo]:
        if self._BaseTypes is None:
            self._init_base_types_if_needed()
        result = []
        for baseType in self._BaseTypes:
            result.extend(baseType.GetAllMethods())
        return result

    @functools.lru_cache(maxsize=128)
    def GetBaseMembers(self, flag:RefTypeFlag=RefTypeFlag.Default) -> List[MemberInfo]:
        if self._BaseTypes is None:
            self._init_base_types_if_needed()
        result = []
        for baseType in self._BaseTypes:
            result.extend(baseType.GetMembers(flag))
        return result

    @functools.lru_cache(maxsize=128)
    def GetAllBaseMembers(self) -> List[MemberInfo]:
        if self._BaseTypes is None:
            self._init_base_types_if_needed()
        result = []
        for baseType in self._BaseTypes:
            result.extend(baseType.GetAllMembers())
        return result

    def GetFields(self, flag:RefTypeFlag=RefTypeFlag.Default) -> List[FieldInfo]:
        self._ensure_initialized()
        if flag == RefTypeFlag.Default:
            result = [field for field in self._FieldInfos
                   if self._where_member(field, RefTypeFlag.Field|RefTypeFlag.Public|RefTypeFlag.Instance)]
        else:
            result = [field for field in self._FieldInfos if self._where_member(field, flag)]
        result.extend(self.GetBaseFields(flag))
        return result

    def GetAllFields(self) -> List[FieldInfo]:
        self._ensure_initialized()
        result = self._FieldInfos.copy()
        result.extend(self.GetAllBaseFields())
        return result

    def GetMethods(self, flag:RefTypeFlag=RefTypeFlag.Default) -> List[MethodInfo]:
        self._ensure_initialized()
        if flag == RefTypeFlag.Default:
            result = [method for method in self._MethodInfos
                   if self._where_member(method, RefTypeFlag.Method|RefTypeFlag.Public|RefTypeFlag.Instance|RefTypeFlag.Static)]
        else:
            result = [method for method in self._MethodInfos if self._where_member(method, flag)]
        result.extend(self.GetBaseMethods(flag))
        return result

    def GetAllMethods(self) -> List[MethodInfo]:
        self._ensure_initialized()
        result = self._MethodInfos.copy()
        result.extend(self.GetAllBaseMethods())
        return result

    def GetMembers(self, flag:RefTypeFlag=RefTypeFlag.Default) -> List[MemberInfo]:
        self._ensure_initialized()
        if flag == RefTypeFlag.Default:
            result = [member for member in self._FieldInfos + self._MethodInfos
                   if self._where_member(member, RefTypeFlag.Public|RefTypeFlag.Instance|RefTypeFlag.Field|RefTypeFlag.Method)]
        else:
            result = [member for member in self._FieldInfos + self._MethodInfos if self._where_member(member, flag)]
        result.extend(self.GetBaseMembers(flag))
        return result

    def GetAllMembers(self) -> List[MemberInfo]:
        self._ensure_initialized()
        result = self._FieldInfos + self._MethodInfos
        result.extend(self.GetAllBaseMembers())
        return result

type RTypen[_T] = RefType
'''
RTypen[T] 是 T 类型的 RefType
'''

_Internal_TypeManager:Optional['TypeManager'] = None

class TypeManager(BaseModel, any_class):
    _RefTypes:              Dict[type|_SpecialIndictaor, RefType]   = PrivateAttr(default_factory=dict)
    _is_preheated:          bool                                    = PrivateAttr(default=False)
    _weak_refs:             Dict[int, "weakref.ref[RefType]"]       = PrivateAttr(default_factory=dict)  # 使用真正的弱引用
    _type_name_cache:       Dict[str, type]                         = PrivateAttr(default_factory=dict)  # 类型名称到类型的缓存
    _string_to_type_cache:  Dict[str, Any]                          = PrivateAttr(default_factory=dict)  # 字符串到类型的缓存

    @classmethod
    def GetInstance(cls) -> Self:
        global _Internal_TypeManager
        if _Internal_TypeManager is None:
            _Internal_TypeManager = cls()
            _Internal_TypeManager._preheat_cache()
        return _Internal_TypeManager

    def _preheat_cache(self):
        """预热缓存，为常用类型预先创建RefType"""
        if self._is_preheated:
            return

        # 常用的基础类型列表
        common_types = [
            int, float, str, bool, list, dict, tuple, set,
            object, type, None.__class__, Exception, BaseModel, any_class
        ]

        # 预加载的类型和它们之间常见的关系，减少后续运行时计算
        for t in common_types:
            self.CreateRefType(t)
            # 同时缓存类型名称到类型的映射
            self._type_name_cache[t.__name__] = t
            # 也缓存类型字符串到类型的映射
            self._string_to_type_cache[t.__name__] = t
            # 缓存模块全限定名
            if t.__module__ != 'builtins':
                full_name = f"{t.__module__}.{t.__name__}"
                self._string_to_type_cache[full_name] = t

        self._is_preheated = True

    def AllRefTypes(self) -> Tuple[RefType, ...]:
        return tuple(self._RefTypes.values())

    @staticmethod
    #@functools.lru_cache(maxsize=256)
    def _TurnToType(data:Any, module_name:Optional[str]=None) -> type|_SpecialIndictaor:
        """将任意数据转换为类型，增加缓存以提高性能"""
        metaType:type|_SpecialIndictaor = None

        # 快速路径：如果已经是类型，直接返回
        if isinstance(data, type) or isinstance(data, _SpecialIndictaor):
            return data

        # 处理字符串类型
        if isinstance(data, str):
            # 尝试使用模块名解析类型
            if module_name is not None:
                try:
                    return sys.modules[module_name].__dict__[data]
                except (KeyError, AttributeError):
                    pass

            # 尝试使用to_type函数
            try:
                return to_type(data, module_name=module_name)
            except Exception:
                pass

        # 尝试使用try_to_type函数作为回退
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
        """创建或获取RefType实例，使用多级缓存提高性能"""
        if data is None:
            raise ReflectionException("data is None")
        if GetInternalReflectionDebug():
            print_colorful(ConsoleFrontColor.YELLOW, f"Try Get RefType: {ConsoleFrontColor.RESET}{data}")

        # 快速路径：如果是字符串并且在字符串缓存中，直接返回对应的类型
        if isinstance(data, str) and data in self._string_to_type_cache:
            data = self._string_to_type_cache[data]

        # 获取或转换为类型
        metaType:type|_SpecialIndictaor = TypeManager._TurnToType(data, module_name=module_name)

        # 首先尝试从弱引用缓存中获取
        type_id = id(metaType)
        if type_id in self._weak_refs:
            ref_type = self._weak_refs[type_id]()
            if ref_type is not None:
                return ref_type
            else:
                # 如果弱引用已被回收，则从字典中删除
                del self._weak_refs[type_id]

        # 然后尝试从常规缓存中获取
        try:
            ref_type = self._RefTypes[metaType]
            # 添加到弱引用缓存
            self._weak_refs[type_id] = weakref.ref(ref_type)
            if GetInternalReflectionDebug():
                print_colorful(ConsoleFrontColor.YELLOW, f"Get "\
                    f"{ConsoleFrontColor.RESET}{metaType}{ConsoleFrontColor.YELLOW} RefType: "\
                    f"{ConsoleFrontColor.RESET}{ref_type.ToString()}")
            return ref_type
        except KeyError:
            ref_type = self.CreateRefType(metaType, module_name=module_name)
            # 添加到弱引用缓存
            self._weak_refs[type_id] = weakref.ref(ref_type)
            return ref_type

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
        """创建新的RefType实例"""
        if data is None:
            raise ReflectionException("data is None")

        # 快速路径：如果是字符串并且在字符串缓存中，直接返回对应的类型
        if isinstance(data, str) and data in self._string_to_type_cache:
            data = self._string_to_type_cache[data]

        metaType:type|_SpecialIndictaor = TypeManager._TurnToType(data, module_name=module_name)

        # 如果是字符串类型，缓存结果以供将来使用
        if isinstance(data, str) and not data in self._string_to_type_cache:
            self._string_to_type_cache[data] = metaType

        try:
            ref_type = RefType(metaType)
            if GetInternalReflectionDebug():
                print_colorful(ConsoleFrontColor.RED, f"Create "\
                    f"{ConsoleFrontColor.RESET}{metaType} "\
                    f"{ConsoleFrontColor.RED}RefType: {ConsoleFrontColor.RESET}{ref_type.ToString()}")
            self._RefTypes[metaType] = ref_type
            return ref_type
        except Exception as e:
            raise ReflectionException(f"Create RefType failed: {e}")

    def CreateOrGetRefTypeFromType(self, type_:type|_SpecialIndictaor) -> RefType:
        """快速路径：直接从类型创建或获取RefType"""
        if type_ in self._RefTypes:
            return self._RefTypes[type_]
        else:
            return self.CreateRefType(type_)
    def CreateRefTypeFromType(self, type_:type|_SpecialIndictaor) -> RefType:
        """直接从类型创建RefType"""
        result = self._RefTypes[type_] = RefType(type_)
        return result


