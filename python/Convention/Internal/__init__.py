from types          import TracebackType
from typing         import *
from abc            import *
import                     sys
import                     threading
import                     traceback
import                     datetime
import                     platform
import                     time
if platform.system() == "Windows":
    from colorama       import Fore as ConsoleFrontColor, Back as ConsoleBackgroundColor, Style as ConsoleStyle

def format_traceback_info(char:str='\n', back:int=1):
    return char.join(traceback.format_stack()[:-back])

INTERNAL_DEBUG = False
def SetInternalDebug(mode:bool):
    global INTERNAL_DEBUG
    INTERNAL_DEBUG = mode
def GetInternalDebug() -> bool:
    global INTERNAL_DEBUG
    return INTERNAL_DEBUG

def print_colorful(color:str, *args, is_reset:bool=True, **kwargs):
    with lock_guard():
        if is_reset:
            print(color,*args,ConsoleStyle.RESET_ALL, **kwargs)
        else:
            print(color,*args, **kwargs)

ImportingFailedSet:Set[str] = set()
def ImportingThrow(
    ex:             ImportError,
    moduleName:     str,
    requierds:      Sequence[str],
    *,
    messageBase:    str = ConsoleFrontColor.RED+"{module} Module requires {required} package."+ConsoleFrontColor.RESET,
    installBase:    str = ConsoleFrontColor.GREEN+"\tpip install {name}"+ConsoleFrontColor.RESET
    ):
    with lock_guard():
        requierds_str = ",".join([f"<{r}>" for r in requierds])
        print(messageBase.format_map(dict(module=moduleName, required=requierds_str)))
        print('Install it via command:')
        for i in requierds:
            global ImportingFailedSet
            ImportingFailedSet.add(i)
            install = installBase.format_map({"name":i})
            print(install)
        if ex:
            print(ConsoleFrontColor.RED, f"Import Error On {moduleName} Module: {ex}, \b{ex.path}\n"\
                f"[{ConsoleFrontColor.RESET}{format_traceback_info(back=2)}{ConsoleFrontColor.RED}]")

def InternalImportingThrow(
    moduleName:     str,
    requierds:      Sequence[str],
    *,
    messageBase:    str = ConsoleFrontColor.RED+"{module} Module requires internal Convention package: {required}."+ConsoleFrontColor.RESET,
    ):
    with lock_guard():
        requierds_str = ",".join([f"<{r}>" for r in requierds])
        print(f"Internal Convention package is not installed.\n{messageBase.format_map({
            "module": moduleName,
            "required": requierds_str
        })}\n[{ConsoleFrontColor.RESET}{format_traceback_info(back=2)}{ConsoleFrontColor.RED}]")

def ReleaseFailed2Requirements():
    global ImportingFailedSet
    if len(ImportingFailedSet) == 0:
        return
    with open("requirements.txt", 'w') as f:
        f.write("\n".join(ImportingFailedSet))

try:
    from pydantic import *
except ImportError:
    InternalImportingThrow("Internal", ["pydantic"])

def virtual[_Func:Callable](func:_Func) -> _Func:
    '''
    你拥有覆写这个函数的权力, 这使得函数成为一个虚函数
    '''
    try:
        if not hasattr(func, "__is_sealed__"):
            func.__is_virtual__ = True
        else:
            raise TypeError("Cannot override a sealed method")
    except AttributeError:
        pass
    return func
def sealed[_Func:Callable](func:_Func) -> _Func:
    '''
    密封这个函数, 这使得函数不可被@virtual装饰
    '''
    try:
        if not hasattr(func, "__is_virtual__"):
            func.__is_sealed__ = True
        else:
            raise TypeError("Cannot sealed a virtual method")
    except AttributeError:
        pass
    return func
def final[_Func:Callable](func:_Func) -> _Func:
    '''
    取消该函数的虚函数特性, 这使得函数不可被继承
    '''
    try:
        if hasattr(func, "__is_virtual__"):
            func.__is_final__ = True
            func.__is_virtual__ = False
        else:
            raise TypeError("Cannot final a un-virtual method")
    except AttributeError:
        pass
    return func
def noexcept[_Func:Callable](func:_Func) -> _Func:
    '''
    该函数将不会抛出异常, 无论发生什么
    '''
    def wrapper(*args, **kwargs):
        try:
            return func(*args, **kwargs)
        except Exception as e:
            return None
    return wrapper
def throw[_Func:Callable](func:_Func, *args:Exception) -> _Func:
    '''
    该函数抛出的异常如果不在预期内, 将终止整个程序,
    预期异常列表为空时, 也视为该函数不应抛出异常
    '''
    def wrapper(*args, **kwargs):
        try:
            return func(*args, **kwargs)
        except Exception as e:
            if e not in args:
                print_colorful(ConsoleFrontColor.RED, f"Unexpected exception: {e}")
                sys.exit(-1)
            else:
                return None
    return wrapper

type Typen[_T] = type

def static_cast[_T](typen:Typen[_T], from_) -> _T:
    return typen(from_)
def dynamic_cast[_T](typen:Typen[_T], from_) -> Optional[_T]:
    if isinstance(from_, typen):
        return from_
    return None
def reinterpret_cast[_T](typen:Typen[_T], from_) -> _T:
    raise NotImplementedError("Python does not support reinterpret_cast anyways")

def any_if[_T](iter:Iterable[_T], pr:Callable[[_T], bool]) -> bool:
    for i in iter:
        if pr(i):
            return True
    return False
def all_if[_T](iter:Iterable[_T], pr:Callable[[_T], bool]) -> bool:
    for i in iter:
        if not pr(i):
            return False
    return True

def first_if[_T](
        iter:       Iterable[_T],
        pr:         Callable[[_T], bool],
        default:    Optional[_T]            = None
        ) -> Optional[_T]:
    for i in iter:
        if pr(i):
            return i
    return default
def first_if_not[_T](
        iter:       Iterable[_T],
        pr:         Callable[[_T], bool],
        default:    Optional[_T]            = None
        ) -> Optional[_T]:
    return first_if(iter, lambda x: not pr(x), default)
def last_if[_T](
        iter:       Iterable[_T],
        pr:         Callable[[_T], bool],
        default:    Optional[_T]            = None
        ) -> Optional[_T]:
    for i in reversed(iter):
        if pr(i):
            return i
    return default
def last_if_not[_T](
        iter:       Iterable[_T],
        pr:         Callable[[_T], bool],
        default:    Optional[_T]            = None
        ) -> Optional[_T]:
    return last_if(iter, lambda x: not pr(x), default)

type Action[_T] = Callable[[_T], None]
type Action2[_T1, _T2] = Callable[[_T1, _T2], None]
type Action3[_T1, _T2, _T3] = Callable[[_T1, _T2, _T3], None]
type Action4[_T1, _T2, _T3, _T4] = Callable[[_T1, _T2, _T3, _T4], None]
type Action5[_T1, _T2, _T3, _T4, _T5] = Callable[[_T1, _T2, _T3, _T4, _T5], None]
type ActionW = Callable[[Sequence[Any]], None]
type ClosuresCallable[_T] = Union[Callable[[Optional[None]], _T], Typen[_T]]

class type_class(object):
    """
    基础类型类，是框架中所有对象类型的基类。

    该类提供了对象类型反射、类型转换、类型检查等基础功能，是框架中类型系统的核心。
    实现了类似C++中的RTTI(运行时类型识别)机制，支持动态类型转换和类型安全检查。

    特性:
    - 提供对象的类型获取和名称解析
    - 支持类型转换操作(AsRef, AsValue)
    - 实现类型检查与条件执行(Is, IfIam)
    - 支持上下文管理器协议(__enter__, __exit__)
    - 提供程序集(Assembly)信息

    当开启内部调试模式时，会自动记录对象创建的堆栈信息，便于追踪对象生命周期。

    继承自该类的子类可以覆盖虚方法来自定义行为，但应当保持类型系统的一致性。
    """
    if GetInternalDebug():
        generate_trackback: Optional[str] = None
        def __init__(self):
            if GetInternalDebug():
                self.generate_trackback = format_traceback_info()
    else:
        def __init__(self):
            pass
    def GetType(self) -> type:
        return type(self)
    @virtual
    def SymbolName(self) -> str:
        return self.GetType().__name__
    @virtual
    def ToString(self) -> str:
        return str(self.GetType())
    def AsRef[_T](self, typen:Typen[_T]) -> _T:
        return dynamic_cast(self, typen)
    def AsValue[_T](self, typen:Typen[_T]) -> _T:
        '''
        warning: this will be a real transform, it is not a reference to the object
        '''
        return static_cast[_T](self, typen)
    def Fetch[_T](self, value:_T) -> _T:
        return value
    def Is[_T](self, typen:Typen[_T]) -> bool:
        return isinstance(self, typen)
    def IfIam[_T](self, typen:Typen[_T], action:Action[_T]) -> Self:
        if self.Is(typen):
            action(self)
        return self
    def AsIam[_T](self, typen:Typen[_T], action:Action[_T]) -> Self:
        action(self.AsRef(typen))
        return self
    @virtual
    def __enter__(self) -> Self:
        return self
    @virtual
    def __exit__(
        self,
        exc_type:   Optional[type],
        exc_val:    Optional[BaseException],
        exc_tb:     Optional[TracebackType]
        ) -> bool:
        if exc_val is None:
            return True
        else:
            return False

    @classmethod
    def class_name(cls) -> str:
        return cls.__name__

    @classmethod
    @virtual
    def Assembly(cls) -> str:
        return "Global"

    @virtual
    def GetAssembly(self) -> str:
        return self.__class__.Assembly()

class base_value_reference[_T](type_class):
    _ref_value:     Optional[_T]        = None
    _real_type:     Optional[type]      = None
    def __init__(self, ref_value:_T):
        super().__init__()
        self._reinit_ref_value(ref_value)
    def _reinit_ref_value(self, value:_T):
        self._ref_value = value
        self._real_type = type(value)
    #def __getattr__(self, name):
    #    try:
    #        return super().__getattr__(name)
    #    except AttributeError:
    #        return self._ref_value.__getattr__(name)
    #def __setattr__(self, name, value):
    #    try:
    #        super().__setattr__(name, value)
    #    except AttributeError:
    #        self._ref_value.__setattr__(name, value)
    def _clear_ref_value(self):
        self._ref_value = None
    @override
    def GetRealType(self):
        return self._real_type
    @override
    def SymbolName(self) -> str:
        if self._ref_value is None:
            return f"{self.GetRealType()}<None, generate_on\n{self.generate_trackback}\n>"
        return f"{self.GetRealType().__name__}<generate on\n{self.generate_trackback}\n>&"
    @override
    def ToString(self) -> str:
        if self._ref_value is None:
            return "None"
        return str(self._ref_value)

    @override
    def AsRef[_T](self, typen:Typen[_T]) -> Optional[_T]:
        '''
        If ref_value is None, using base class AsRef,
        If ref_value is drivered by typen, return ref_value,
        Else dynamic_cast ref_value to typen or self to typen
        '''
        if self.GetRealType() == typen or isinstance(self._ref_value, typen):
            return self._ref_value
        result = super().AsRef(typen)
        if result is None and self._ref_value is not None:
            result = dynamic_cast(typen, self._ref_value)
        return result
    @override
    def AsValue[_T](self, typen:Typen[_T]) -> _T:
        if self.GetRealType() == typen or isinstance(self._ref_value, typen):
            return self._ref_value
        else:
            return super().AsValue(typen)
    @overload
    def Is[_T](self, typen:Typen[_T]) -> bool:
        return self.GetRealType() == typen or isinstance(self._ref_value, typen) or super().Is(typen)
    @overload
    def IfIam[_T](self, typen:Typen[_T], action:Action[_T]) -> Self:
        if self.GetRealType() == typen or isinstance(self._ref_value, typen):
            action(self._ref_value)
        else:
            action(self)
        return self

    @virtual
    def __repr__(self):
        if self._ref_value is None:
            return f"{self.GetRealType()}<None>"
        return f"{self.GetRealType()}&"
    @virtual
    def __str__(self):
        if self._ref_value is None:
            return "None"
        return f"{self._ref_value}"
class left_value_reference[_T](base_value_reference):
    def __init__(self, ref_value:_T):
        super().__init__(ref_value)
    @property
    def ref_value(self) -> _T:
        return self._ref_value
    @ref_value.setter
    def ref_value(self, value) -> _T:
        if self.GetRealType() is None or self.GetRealType() == type(None):
            self._reinit_ref_value(value)
        elif value is None or isinstance(value, self.GetRealType()):
            self._ref_value = value
        else:
            raise TypeError(f"Cannot assign {type(value)} to {self.GetRealType()}")
        return value
    def __bool__(self):
        return self._ref_value is not None
    def is_empty(self):
        return self._ref_value is None
class right_value_refenence[_T](base_value_reference):
    def __init__(self, ref_value:_T):
        super().__init__(ref_value)
    @property
    def ref_value(self) -> _T:
        result = self._ref_value
        self._ref_value = None
        return result
    @property
    def const_ref_value(self) -> _T:
        return self._ref_value
class any_class(type_class, ABC):
    """
    抽象基类，继承自type_class，是框架中所有高级对象的共同基类。

    该类是一个抽象类(ABC)，提供了更高级的对象共享和程序集管理功能。
    设计为框架中各种服务、组件和实体的基础接口，定义了标准行为和约定。

    特性:
    - 继承自type_class的所有基础类型系统功能
    - 提供对象共享机制(Share方法)，支持引用传递
    - 指定特定的程序集标识(Convention.Runtime)
    - 作为抽象基类，可以定义接口约定

    在框架中，any_class充当了标准接口的角色，使得不同组件可以通过多态性进行交互，
    同时维护类型安全和引用完整性。服务定位器、事件系统等高级功能通常基于此类实现。

    使用该类作为基类可以确保对象遵循框架的约定和标准，便于系统集成和扩展。
    """
    def __init__(self):
        super().__init__()
    def Share[_T](self, out_value:left_value_reference[_T]) -> Self:
        if out_value is None:
            raise ValueError("out_value cannot be None")
        if isinstance(self, out_value.GetRealType()):
            out_value.ref_value = self
        else:
            out_value.ref_value = out_value.GetRealType()(self)
        return self

    @classmethod
    @override
    def Assembly(cls) -> str:
        return "Convention.Runtime"

def UnwrapperInstance2Ref[_T](instance:Union[_T, base_value_reference[_T]]) -> _T:
    if isinstance(instance, base_value_reference):
        return instance.ref_value
    else:
        return instance

def AssemblyTypen(obj:Any) -> str:
    if isinstance(obj, type):
        return f"{obj.__module__}.{obj.__name__}, "\
            f"{obj.Assembly() if hasattr(obj, "Assembly") else "Global"}"
    else:
        return f"{obj.__class__.__module__}.{obj.__class__.__name__}, "\
            f"{obj.GetAssembly() if hasattr(obj, "GetAssembly") else "Global"}"
def ReadAssemblyTypen(
    assembly_typen: str,
    *,
    premodule:      Optional[str|Callable[[str], str]] = None
    ) -> Tuple[type, str]:
    typen, assembly_name = assembly_typen.split(",")
    module_name, _, class_name = typen.rpartition(".")
    if premodule is not None:
        if isinstance(premodule, str):
            module_name = premodule
        else:
            module_name = premodule(module_name)
    import importlib
    target_type = getattr(importlib.import_module(module_name), class_name)
    return target_type, assembly_name

class invoke_callable(any_class):
    def __init__(self):
        super().__init__()
    def __call__(self, *args, **kwargs):
        if "invoke" in dir(self):
            return self.invoke(*args, **kwargs)
        else:
            raise NotImplementedError(f"self<{self.SymbolName()}> invoke not implemented")
class null_package[_T](left_value_reference[_T]):
    @override
    def __init__(self, ref_value:_T):
        super().__init__(ref_value)

    def Try[_T](
        self,
        typen:Typen[_T],
        call:Action[_T]
        ) -> Self:
        if self.GetRealType() == typen:
            call(self.ref_value)
        return self
class closures[_T](left_value_reference[_T], invoke_callable):
    @override
    def __init__(self, ref_value:_T, callback:Action[_T]):
        super().__init__(ref_value)
        self.callback = callback
    def invoke(self):
        if self.callback is None:
            return
        self.callback(self.ref_value)
        self.callback = None
class release_closures[_T](closures[_T]):
    @override
    def __init__(self, ref_value:_T, callback:Action[_T]):
        super().__init__(ref_value)
        self.callback = callback
    def __del__(self):
        self.invoke()

# LightDiagram::ld::instance<_Ty>
class restructor_instance[_Ty](left_value_reference[_Ty]):
    def __init__(
        self,
        target:             _Ty,
        *,
        constructor_func:   Optional[Callable[[_Ty], None]] = None,
        destructor_func:    Optional[Callable[[_Ty], None]] = None
        ):
        super().__init__(target)
        if constructor_func:
            constructor_func(self.ref_value)
        self.destructor_func = destructor_func
    def __del__(self):
        if self.destructor_func:
            self.destructor_func(self.ref_value)

    def get_ref(self):
        return self.ref_value
    def is_empty(self):
        return self.ref_value is None

class iter_builder[_T](any_class):
    def __init__(self,  pr:Callable[[], bool], returner:Callable[[], _T]):
        self.pr = pr
        self.returner = returner
    def __iter__(self):
        return self
    def __next__(self):
        if self.pr():
            return self.returner()
        raise StopIteration
class iter_callable_range(Callable[[], bool], any_class):
    def __init__(
        self,
        start:  Union[int, left_value_reference[int]],
        stop:   Union[int, left_value_reference[int]],
        step=1
        ):
        self.__start = start
        self.__stop = stop
        self.__start_is_real_value = isinstance(start, int)
        self.__stop_is_real_value = isinstance(stop, int)
        self.step = step
    @property
    def start(self):
        return self.__start if self.__start_is_real_value else self.__start.ref_value
    @start.setter
    def start(self, value:int):
        if self.__start_is_real_value:
            self.__start = value
        else:
            self.__start.ref_value = value
    @property
    def stop(self):
        return self.__stop if self.__stop_is_real_value else self.__stop.ref_value
    @stop.setter
    def stop(self, value:int):
        if self.__stop_is_real_value:
            self.__stop = value
        else:
            self.__stop.ref_value = value
    def __call__(self):
        start = self.start
        stop = self.stop
        result = start<stop
        self.start = start + self.step
        return result

# using as c#: func(out var obj)
_out_static_value_reference = []
class out_value_reference[_T](left_value_reference[_T]):
    '''
    用于作为输出参数，使用时需要使用out_value_reader来读取输出值,
    顺序压参数入栈
    '''
    def __init__(self, value:Optional[_T] = None):
        super().__init__(value)
    def __del__(self):
        _out_static_value_reference.append(self.ref_value)
class out_value_reader[_T](right_value_refenence[_T]):
    '''
    用于读取对应out_value_reference的输出值,
    顺序弹参数出栈
    '''
    def __init__(self):
        if len(_out_static_value_reference) != 0:
            super().__init__(_out_static_value_reference.pop())
        else:
            super().__init__(None)

# using as c#: event
class ActionEvent[_Call:Callable](invoke_callable):
    def __init__(self, actions:Sequence[_Call]):
        super().__init__()
        self.__actions:     List[Callable]  = [action for action in actions]
        self.call_indexs:   List[int]       = [i for i in range(len(actions))]
        self.last_result:   List[Any]       = []
    def call_func_without_call_index_control(self, index:int, *args, **kwargs) -> Union[Any, Exception]:
        try:
            return self.__actions[index](*args, **kwargs)
        except Exception as ex:
            return ex
    def call_func(self, index:int, *args, **kwargs) -> Union[Any, Exception]:
        return self.call_func_without_call_index_control(self.call_indexs[index], *args, **kwargs)
    def _inject_invoke(self, *args, **kwargs):
        result:List[Any] = []
        for index in range(self.call_max_count):
            result.append(self.call_func(index, *args, **kwargs))
        return result
    def invoke(self, *args, **kwargs) -> Union[Self, bool]:
        self.last_result = self._inject_invoke(*args, **kwargs)
        return self
    def init_call_index(self):
        self.call_indexs = [i for i in range(len(self.__actions))]
    def add_action(self, action:_Call):
        self.__actions.append(action)
        self.call_indexs.append(len(self.__actions)-1)
        return self
    def add_actions(self, actions:Sequence[_Call]):
        for action in actions:
            self.add_action(action)
        return self
    def _internal_remove_action(self, action:_Call):
        if action in self.__actions:
            index = self.__actions.index(action)
            self.__actions.remove(action)
            self.call_indexs.remove(index)
            for i in range(len(self.call_indexs)):
                if self.call_indexs[i] > index:
                    self.call_indexs[i] -= 1
            return True
        return False
    def remove_action(self, action:_Call):
        while self._internal_remove_action(action):
            pass
        return self
    def is_valid(self):
        return not any_if(self.last_result, lambda x: isinstance(x, Exception))
    def __bool__(self):
        return self.is_valid()
    @property
    def call_max_count(self):
        return len(self.call_indexs)
    @property
    def action_count(self):
        return len(self.__actions)

# region instance

# threads

class atomic[_T](any_class):
    _value: _T
    locker: threading.Lock
    _is_in_with: bool

    def __init__(
        self,
        value:  Optional[_T]                = None,
        locker: Optional[threading.Lock]    = None,
        ) -> None:
        self._value:        _T              = value
        self._is_in_with:   bool            = False
        self.locker:        threading.Lock  = locker
    def fetch_add(self, value:_T):
        with lock_guard(self.locker):
            self._value += value
        return self._value
    def fetch_sub(self, value:_T):
        with lock_guard(self.locker):
            self._value -= value
        return self._value
    def load(self) -> _T:
        with lock_guard(self.locker):
            return self._value
    def store(self, value: _T):
        with lock_guard(self.locker):
            self._value = value
    def __add__(self, value:_T):
        return self.fetch_add(value)
    def __sub__(self, value:_T):
        return self.fetch_sub(value)
    def __iadd__(self, value:_T):
        self.fetch_add(value)
        return self
    def __isub__(self, value:_T):
        self.fetch_sub(value)
        return self
    @override
    def __enter__(self) -> Self:
        self._is_in_with = True
        self.locker.acquire()
        return self
    @override
    def __exit__(
        self,
        exc_type:   Optional[type],
        exc_val:    Optional[BaseException],
        exc_tb:     Optional[TracebackType]
        ) -> Optional[bool]:
        self._is_in_with = False
        self.locker.release()
        if exc_type is None:
            return True
        else:
            raise exc_val
    @property
    def value(self) -> _T:
        if self._is_in_with:
            return self._value
        raise NotImplementedError("This method can only be called within a with statement")
    @value.setter
    def value(self, value:_T) -> _T:
        if self._is_in_with:
            self._value = value
        raise NotImplementedError("This method can only be called within a with statement")

    def __iadd__(self, value):
        self.fetch_add(value)
        return self
    def __isub__(self, value):
        self.fetch_sub(value)
        return self
    def __str__(self) -> str:
        return str(self.load())
    def __repr__(self) -> str:
        return repr(self.load())

    def SymbolName(self) -> str:
        return "atomic"

InternalGlobalLocker = threading.Lock()
InternalGlobalLockerCount = atomic[int](0)

class lock_guard(any_class):
    _locker:        Union[threading._RLock, threading.Lock]

    def __init__(
        self,
        lock:   Optional[Union[threading.RLock, threading.Lock]] = None
        ):
        if lock is None:
            lock = InternalGlobalLocker
        self._locker = lock
        self._locker.acquire()
    def __del__(self):
        self._locker.release()

class global_lock_guard(lock_guard):
    def __init__(self):
        super().__init__(None)

class thread_instance(threading.Thread, any_class):
    def __init__(
        self,
        call:           Action[None],
        *,
        is_del_join:    bool = True
        ):
        super().__init__(target=call)
        self.is_del_join = is_del_join
        self.start()
    def __del__(self):
        if self.is_del_join:
            self.join()

# region end

def create_py_file(path:str):
    with open(path, "w") as f:
        f.write("# -*- coding: utf-8 -*-\n")
        f.write("from Convention import lazy\n")
        f.write("from Convention.lazy import *\n")
        f.write("\n")
        f.write("def run():\n")
        f.write("\tpass\n")
        f.write("\n")
        f.write(f"if __name__ == \"__main__\":\n")
        f.write("\trun()\n")

def WrapperConfig2Instance[_TargetType](
    typen_or_generater:             Union[type, Callable[[Any], _TargetType]],
    datahead_of_config_or_instance: Optional[Union[
            Dict[str, Any], # kwargs
            Sequence[Any],  # args
            _TargetType,            # typen or datahead
        ]],
    *args,
    datahead_typen_:                 Optional[type] = None,
    **kwargs
    ) -> _TargetType:
    if typen_or_generater is None:
        raise ValueError("typen cannt be none")
    if datahead_of_config_or_instance is None:
        return typen_or_generater(*args, **kwargs)
    elif datahead_typen_ is not None and isinstance(datahead_of_config_or_instance, datahead_typen_):
        return datahead_of_config_or_instance
    elif isinstance(typen_or_generater, type) and isinstance(datahead_of_config_or_instance, typen_or_generater):
        return datahead_of_config_or_instance
    elif isinstance(datahead_of_config_or_instance, dict):
        return typen_or_generater(*args, **datahead_of_config_or_instance, **kwargs)
    elif isinstance(datahead_of_config_or_instance, Sequence):
        return typen_or_generater(*datahead_of_config_or_instance, *args, **kwargs)
    else:
        return typen_or_generater(datahead_of_config_or_instance, *args, **kwargs)

def remove_same_value[_T:Union[
        tuple, list
    ]](data:_T) -> _T:
    return type(data)(set(data))
def remove_none_value[_T:Union[
        dict, tuple, list
    ]](data:_T) -> _T:
    if isinstance(data, dict):
        return {k: v for k, v in data.items() if v is not None}
    elif isinstance(data, (tuple, list)):
        return type(data)(v for v in data if v is not None)
    else:
        raise ValueError(f"remove_none_value not support this type<{type(data)}>")
def to_list[_DataTy, _T:Sequence[_DataTy]](data:_T) -> List[_DataTy]:
    return data if isinstance(data, list) else list(data)
def to_tuple[_DataTy, _T:Sequence[_DataTy]](data:_T) -> Tuple[_DataTy, ...]:
    return data if isinstance(data, tuple) else tuple(data)

def nowf() -> str:
    '''
    printf now time to YYYY-MM-DD_HH-MM-SS format,
    return: str
    '''
    return datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")

_all_base_behavior:List['BaseBehavior'] = []
_LazyBroadcostEvent: Dict[str, List[Any]] = {}
class BaseBehavior(any_class):
    @virtual
    def OnUpdate(self) -> None:
        '''
        尽可能的在每一个逻辑tick中调用
        '''
        pass
    @virtual
    def OnFixedUpdate(self) -> None:
        '''
        尽可能的在每一个定时的物理tick中调用
        '''
        pass
    @virtual
    def OnLateUpdate(self) -> None:
        '''
        尽可能的在每一个逻辑tick的末尾调用
        '''
        pass
    @sealed
    def Broadcast(self, event:Any, funcname:str|Literal["OnEvent"] = "OnEvent") -> None:
        '''
        向所有Behavior广播事件

        Args:
            event: 事件
            funcname: 函数名
        '''
        if GetInternalDebug():
            print_colorful(ConsoleFrontColor.YELLOW, f"广播事件: {funcname}, 事件类型: {type(event)}")
        global _LazyBroadcostEvent
        if funcname in _LazyBroadcostEvent:
            _LazyBroadcostEvent[funcname].append(event)
        else:
            _LazyBroadcostEvent[funcname] = [event]

    @virtual
    def OnEvent(self, event:Any) -> None:
        '''
        处理事件使用的默认函数, 该函数无法保证调用顺序因此不要在其中执行逻辑
        '''
        pass
    def __init__(self):
        global _all_base_behavior
        _all_base_behavior.append(self)
    def __del__(self):
        global _all_base_behavior
        _all_base_behavior.remove(self)

_behavior_thread:Optional[thread_instance] = None
_behavior_thread_fixed_update_delta_time:float = 1/60
_behavior_thread_is_running:bool = False
def _GetBehaviorThreadIsRunningStats() -> bool:
    global _behavior_thread_is_running
    return _behavior_thread_is_running
def _SetBehaviorThreadIsRunningStats(is_running:bool):
    global _behavior_thread_is_running
    _behavior_thread_is_running = is_running

_behavior_debug_hook:Optional[Action[None]] = None
_behavior_exception_hook:Optional[Action[Exception]] = None

def SetBehaviorDebugHook(hook:Optional[Action[None]]):
    global _behavior_debug_hook
    _behavior_debug_hook = hook

def SetBehaviorExceptionHook(hook:Optional[Action[Exception]]):
    global _behavior_exception_hook
    _behavior_exception_hook = hook

def AwakeBehaviorThread(*, fixedUpdateDeltaTime:float=1/60):
    '''
    唤醒一个线程用于运行所有BaseBehavior的生命周期方法,
    try OnFixedUpdate -- OnUpdate -- try OnFixedUpdate -- OnLateUpdate -- 循环
    OnFixedUpdate会尝试总在固定的时间间隔后进行调用

    返回:
        thread_instance, 该线程不会自动结束, 需要手动结束
    '''
    global _behavior_thread
    global _behavior_thread_fixed_update_delta_time

    if _behavior_thread is not None:
        raise RuntimeError("BehaviorThread already exists")
    def runner():
        clock: left_value_reference[float] = left_value_reference[float](time.time())
        global _all_base_behavior
        def try_fixed_update():
            if time.time() - clock.ref_value >= _behavior_thread_fixed_update_delta_time:
                clock.ref_value = time.time()
                for behavior in _all_base_behavior:
                    try:
                        behavior.OnFixedUpdate()
                    except Exception as e:
                        if _behavior_debug_hook is not None:
                            _behavior_debug_hook(e)
                        if _behavior_exception_hook is not None:
                            _behavior_exception_hook(e)
                        else:
                            _SetBehaviorThreadIsRunningStats(False)
                            raise
        def broadcast_events():
            current_broadcast_events = _LazyBroadcostEvent.copy()
            if GetInternalDebug() and len(current_broadcast_events) > 0:
                print_colorful(ConsoleFrontColor.YELLOW, f"以下事件正在广播: {current_broadcast_events}")
            _LazyBroadcostEvent.clear()
            for funcname, events in current_broadcast_events.items():
                for event in events:
                    for behavior in _all_base_behavior:
                        try:
                            if hasattr(behavior, funcname):
                                getattr(behavior, funcname)(event)
                        except Exception as e:
                            if _behavior_debug_hook is not None:
                                _behavior_debug_hook(e)
                            if _behavior_exception_hook is not None:
                                _behavior_exception_hook(e)
                            else:
                                _SetBehaviorThreadIsRunningStats(False)
                                raise
        while _GetBehaviorThreadIsRunningStats():
            for behavior in _all_base_behavior:
                try:
                    behavior.OnUpdate()
                except Exception as e:
                    if _behavior_debug_hook is not None:
                        _behavior_debug_hook(e)
                    if _behavior_exception_hook is not None:
                        _behavior_exception_hook(e)
                    else:
                        _SetBehaviorThreadIsRunningStats(False)
                        raise
            try_fixed_update()
            for behavior in _all_base_behavior:
                try:
                    behavior.OnLateUpdate()
                except Exception as e:
                    if _behavior_debug_hook is not None:
                        _behavior_debug_hook(e)
                    if _behavior_exception_hook is not None:
                        _behavior_exception_hook(e)
                    else:
                        _SetBehaviorThreadIsRunningStats(False)
                        raise
            try_fixed_update()
            # 延迟执行广播事件
            broadcast_events()
    global INTERNAL_DEBUG
    if INTERNAL_DEBUG:
        print_colorful(ConsoleFrontColor.GREEN, "唤醒生命周期线程")
    _SetBehaviorThreadIsRunningStats(True)
    _behavior_thread = thread_instance(runner, is_del_join=False)
    _behavior_thread_fixed_update_delta_time = fixedUpdateDeltaTime

def StopBehaviorThread():
    global _behavior_thread
    global INTERNAL_DEBUG
    if INTERNAL_DEBUG:
        print_colorful(ConsoleFrontColor.GREEN, "停止生命周期线程")
        print_colorful(ConsoleFrontColor.GREEN, f"生命周期活动中的行为仍有:")
        for behavior in _all_base_behavior:
            print_colorful(ConsoleFrontColor.GREEN, f"\t{behavior.__class__.__name__}"\
                f"<symbol={behavior.SymbolName()}, assembly={behavior.Assembly()}>: "\
                f"{behavior.ToString()}")
    _SetBehaviorThreadIsRunningStats(False)
    if _behavior_thread is not None:
        _behavior_thread.join()
        _behavior_thread = None

# region Engine

class Vector2(BaseModel, any_class):
    x: float = Field(default=0.0)
    y: float = Field(default=0.0)

    def __init__(self, x:float=0.0, y:float=0.0):
        super().__init__()
        self.x = x
        self.y = y

    @classmethod
    def __easy_serialize__(cls, instance:Any) -> Tuple[Dict[str, Any], bool]:
        '''
        序列化

        返回值:
            Dict[str, Any]: 序列化后的数据
            bool: 是否需要添加__type字段
        '''
        return {
            "x": instance.x,
            "y": instance.y
        }, False
    @classmethod
    def __easy_deserialize__(cls, data:Dict[str, Any]):
        return cls(x=data["x"], y=data["y"])

class Vector3(BaseModel, any_class):
    x: float = Field(default=0.0)
    y: float = Field(default=0.0)
    z: float = Field(default=0.0)

    def __init__(self, x:float=0.0, y:float=0.0, z:float=0.0):
        super().__init__()
        self.x = x
        self.y = y
        self.z = z

    @classmethod
    def __easy_serialize__(cls, instance:Any) -> Tuple[Dict[str, Any], bool]:
        '''
        序列化

        返回值:
            Dict[str, Any]: 序列化后的数据
            bool: 是否需要添加__type字段
        '''
        return {
            "x": instance.x,
            "y": instance.y,
            "z": instance.z
        }, False
    @classmethod
    def __easy_deserialize__(cls, data:Dict[str, Any]):
        return cls(x=data["x"], y=data["y"], z=data["z"])

# endregion
