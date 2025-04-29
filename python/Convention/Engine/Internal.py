'''
<编辑环境>:
    生命周期对象的在此环境下不会执行<更新>系列生命周期方法
    在此环境中可以取得<编辑权限>, 可以对对象进行编辑, 可以转移至<构建环境>

    <层级视图>:
        <编辑环境>下查找对象的中间件


<构建环境>:
    生命周期对象的在此环境下不会执行<更新>系列生命周期方法
    在此环境中不可进行任何操作, 自动开展构建任务, 在构建完成后自动转移至<运行环境>

<运行环境>:
    生命周期对象的在此环境下会执行<更新>系列生命周期方法
    在此环境中运行的对象进入正式的运行时, 部分操作将会受到限制

<通用部分>:
    <检视>:
        通过<检视>可以操作对象的属性, 并将本次操作记录到<历史>中

    <历史>:
        通过<历史>可以查看通过<检视>执行的记录, 并进行撤销和重做

'''

from enum import Enum
from ..Internal import *

class _Environment(BaseModel, any_class):
    _Name: str = PrivateAttr(default="")
    _IsEditor: bool = PrivateAttr(default=False)
    _IsBuild: bool = PrivateAttr(default=False)

    @property
    def IsEditor(self) -> bool:
        return self._IsEditor
    @property
    def IsBuild(self) -> bool:
        return self._IsBuild
    @property
    def IsRuntime(self) -> bool:
        return not self._IsEditor and not self._IsBuild

    def __init__(self, name:str, is_editor:bool=False, is_build:bool=False):
        BaseModel.__init__(self)
        any_class.__init__(self)
        self._Name = name
        self._IsEditor = is_editor
        self._IsBuild = is_build

    @virtual
    def __call__(self) -> None:
        pass

class _Editor(_Environment):
    def __init__(self):
        super().__init__("Editor", is_editor=True)

class _Building(_Environment):
    def __init__(self):
        super().__init__("Build", is_build=True)

    def StartBuild(self, editor: _Editor):
        pass

class _Runtime(_Environment):
    def __init__(self):
        super().__init__("Runtime", is_editor=False, is_build=False)

    def ClearRuntimeEngine(self):
        pass

_GlobalEditor: _Editor = _Editor()
_CurrentEnvironment: _Environment = _GlobalEditor
_CurrentMainEngineStats: bool = False

def _ClearRuntimeEngine():
    global _CurrentEnvironment
    if isinstance(_CurrentEnvironment, _Runtime):
        _CurrentEnvironment.ClearRuntimeEngine()

def _DeleteRuntimeEngine():
    global _CurrentEnvironment
    if isinstance(_CurrentEnvironment, _Runtime):
        del _CurrentEnvironment

def EnterEditor():
    global _CurrentEnvironment
    _ClearRuntimeEngine()
    _DeleteRuntimeEngine()
    _CurrentEnvironment = _GlobalEditor

def StartBuild(editor: _Editor):
    global _CurrentEnvironment
    building = _Building()
    building.StartBuild(editor)
    _CurrentEnvironment = building

def SetEngineActive(active:bool):
    global _CurrentMainEngineStats
    _CurrentMainEngineStats = active

def _MainEngineUpdate():
    if _CurrentEnvironment is not None:
        _CurrentEnvironment()

def _MainLoopEntry():
    while True:
        if _CurrentMainEngineStats:
            _MainEngineUpdate()

_BehaviourThread = thread_instance(_MainLoopEntry, is_del_join=False)

def _GetMainThreadID() -> int:
    return _BehaviourThread.ident

class HideFlags(Enum):
    '''
    对象隐藏标志枚举类，用于控制对象在编辑环境中的可见性和行为。

    该枚举定义了多种标志位，可以通过位运算组合使用，用于控制对象在<编辑环境>中的显示、
    保存和编辑行为。这些标志主要用于<编辑环境>开发，对运行时行为没有影响。

    每个标志位使用2的幂次方值，便于进行位运算组合。例如：
    - HideInHierarchy | HideInInspector 表示对象在层级视图和检视面板中都不可见
    - DontSaveInEditor | DontSaveInBuild 表示对象在编辑器和构建时都不会被保存

    注意：这些标志主要用于<编辑环境>开发，对运行时行为没有影响。
    '''

    ''' 一个正常、可见的对象。这是默认值。'''
    Default = 0,
    ''' 对象不会出现在<层级视图>中。'''
    HideInHierarchy = 1,
    ''' 无法在<检视>中查看它。'''
    HideInInspector = 2,
    ''' 对象不会在<编辑环境>中保存到场景。'''
    DontSaveInEditor = 4,
    ''' 对象在<检视>中不可编辑。'''
    NotEditable = 8,
    ''' 对象不会在<构建环境>中保存。'''
    DontSaveInBuild = 0x10,
    ''' 对象不会在资源卸载时被卸载。'''
    DontUnloadUnusedAsset = 0x20,
    ''' 对象不会在构建时保存。它不会在加载新场景时被销毁。
        这是 HideFlags.DontSaveInBuild | HideFlags.DontSaveInEditor
        HideFlags.DontUnloadUnusedAsset 的快捷方式。'''
    DontSave = 0x34,
    ''' 对象不会在<层级视图>中显示，不会保存到场景，也不会被资源卸载。'''
    HideAndDontSave = 0x3D

class CEObject(BaseModel, any_class):
    '''
    Convention Engine Object

    生命周期引擎对象的基类, 所有需要实现生命周期的对象都继承自该类
    '''
    def __init__(self, *args, **kwargs):
        BaseModel.__init__(self, *args, **kwargs)
        any_class.__init__(self)

    _CachedPtr:                             int = PrivateAttr(default=-1)
    _InstanceID:                            int = PrivateAttr(default=-1)
    _EngineRuntimeErrorString:              str = PrivateAttr(default="")
    _OffsetOfInstanceIDInCPlusPlusObject:   int = PrivateAttr(default=-1)
    _ObjectIsNullMessage:                   str = PrivateAttr(default="将实例化的对象为空。")
    _CloneDestroyedMessage:                 str = PrivateAttr(default="实例化失败，因为克隆体在创建过程中被销毁。"\
                                                                      "如果Behaviour.Awake中调用了DestroyImmediate，可能会发生这种情况。")

    _Name:                                  str = PrivateAttr(default="")
    _HideFlags:                       HideFlags = PrivateAttr(default=HideFlags.Default)

    @property
    def name(self) -> str:
        return CEObject._GetName(self)
    @name.setter
    def name(self, value:str):
        CEObject._SetName(self, value)

    @staticmethod
    def _GetName(obj:'CEObject') -> str:
        return obj._Name
    @staticmethod
    def _SetName(obj:'CEObject', name:str):
        obj._Name = name

    @sealed
    def _GetCachedPtr(self) -> int:
        return self._CachedPtr

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

    @override
    def GetHashCode(self) -> int:
        return self._InstanceID

    @virtual
    def __eq__(self, other:'CEObject') -> bool:
        if other is not CEObject:
            return False
        return

    @sealed
    def EnsureRunningOnMainThread(self):
        if not threading.current_thread().ident == _GetMainThreadID():
            raise RuntimeError(f"{self.EnsureRunningOnMainThread.__name__}必须在引擎的主线程中执行")

    @staticmethod
    @sealed
    def _IsNativeObjectAlive(o:'CEObject') -> bool:
        if o._GetCachedPtr() != 0:
            return True
        return CEObject.DoesObjectWithInstanceIDExist(o._GetInstanceID())

    @staticmethod
    @sealed
    def _CompareBaseObject(lhs:Optional['CEObject'], rhs:Optional['CEObject']) -> bool:
        if lhs is None and rhs is None:
            return True
        if rhs is None:
            return not CEObject._IsNativeObjectAlive(lhs)
        if lhs is None:
            return not CEObject._IsNativeObjectAlive(rhs)
        return lhs._InstanceID == rhs._InstanceID

    @staticmethod
    @sealed
    def DoesObjectWithInstanceIDExist(instance_id:int) -> bool:
        raise NotImplementedError()


