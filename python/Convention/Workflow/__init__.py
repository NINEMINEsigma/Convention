from ..Internal         import *
from ..Lang.Core        import run_async_coroutine
from ..Lang.Reflection  import MethodInfo
from pydantic           import BaseModel, Field, GetCoreSchemaHandler
from pydantic_core      import core_schema
from ..Lang.EasySave    import EasySave, SetInternalEasySaveDebug, SetInternalDebug, SetInternalReflectionDebug
from ..Str.Core         import UnWrapper as Unwrapper2Str, limit_str
from ..File.Core        import (
    tool_file_or_str    as     tool_file_or_str,
    tool_file           as     tool_file,
    Wrapper             as     Wrapper2File,
)
from ..Web.Core         import (
    tool_url            as     tool_url,
)
import                         time
import                         asyncio

_InternalWorkflowDebug:bool = False
def SetInternalWorkflowDebug(debug:bool):
    global _InternalWorkflowDebug
    _InternalWorkflowDebug = debug
def GetInternalWorkflowDebug() -> bool:
    return _InternalWorkflowDebug and GetInternalDebug()

_workflow_manager_instance: Optional['WorkflowManager'] = None
_Internal_GetNodeID: Callable[['Node'], int] = lambda node: _workflow_manager_instance.GetNodeID(node)
_Internal_GetNode: Callable[[int], 'Node'] = lambda id: _workflow_manager_instance.GetNode(id)
_Internal_ContainsNode: Callable[[int], bool] = lambda id: _workflow_manager_instance.ContainsNode(id)

context_type = Dict[str, Any]
context_key_type = str
context_value_type = Any
context_keyword = Literal["result", "error"]
'''
上下文中特殊的键值对:
    result: 步骤的输出结果(当步骤执行结果不是一个字典时, 结果将被保存到此键值对上)
    error: 步骤的错误信息(当步骤执行失败时, 错误信息将被保存到此键值对上)
'''
input_mapping_type = Dict[str, str]
output_mapping_type = Dict[str, str]
action_label_type = str

action_name:Literal["action"] = "action" # 动作名称

'''
工作流的整个构建流程如下:
---
    开始节点(只有输出映射, 自身能够加载本地或云端资源或实例作为输出参数初始化上下文参数)

    上游步骤节点(输入映射可选, 有输出映射, 通过获取上游节点的参数, 调用函数, 输出并更新上下文,
            没有输入的步骤节点与开始节点等同, 但是自身不能加载资源或实例, 只能通过函数的返回值生成输出)

    下游步骤节点(有输入映射与输出映射, 通过获取上游节点的参数, 调用函数, 输出并更新上下文)

    终止节点(只有输入映射, 获取上游节点的参数, 汇总后生成本节点的内容, 本节点将展示最终输出结果)

工作流的整个命令与事件传递流程如下:
---
    终止节点:
        开始尝试获取上游节点参数, 广播StartEvent, 接收到全部参数后构建最终结果,
        当全部终止节点都完成后广播StopEvent

    上游步骤节点:
        相关的输出槽被获取时, 开始获取上游节点参数, 接收到全部参数后, 调用函数, 广播StepEvent, 输出并更新上下文,
        接收到StopEvent后, 停止

    开始节点:
        接收到StartEvent后立即加载资源, 无论是否最终连接到终止节点,
        接收到StopEvent后, 停止

工作流中两个节点的连接:
---
    上游节点 -> 上游节点-输出槽 -> 下游节点-输入槽 -> 下游节点
    下游节点获取参数字典时从输入槽获取, 输出槽从上游节点输出槽获取, 上游节点输出槽的参数由上游节点设置

WorkflowManager (工作流管理器)
├── Workflow (工作流信息)
│   ├── NodeInfo (节点信息)
│   │   ├── NodeSlotInfo (插槽信息)
│   │   └── inmapping/outmapping (输入/输出映射)
│   └── Nodes (节点列表)
├── Node (节点基类)
│   ├── ResourceNode (资源节点)
│   ├── TextNode (文本节点)
│   ├── ...
│   ├── StepNode (步骤节点)
│   └── EndNode (结束节点)
└── WorkflowEvent (工作流事件基类)
    ├── WorkflowStartEvent (开始事件)
    ├── WorkflowStopEvent (停止事件)
    └── WorkflowErrorEvent (错误事件)

'''


class NodeSlotInfo(BaseModel, any_class):
    """
    插槽信息

    初始化的Link由Node执行, 并且两个槽必须互相连接
    """
    parentNode:         'Node'     = Field(description="所属的父节点", default=None, exclude=True)
    slot:               'NodeSlot' = Field(description="所属的插槽", default=None, exclude=True)
    slotName:           str        = Field(description="插槽名称", default="unknown")
    targetNode:         'Node'     = Field(description="目标节点, 此变量需要手动同步, targetNodeID的懒加载目标,"\
        "对于输入节点而言, 这个节点是唯一的目标节点, 对于输出节点而言, 这个节点是最后被连接的目标节点",
                                           default=None, exclude=True)
    targetSlot:         'NodeSlot' = Field(description="目标插槽, 此变量需要手动同步, targetSlotName的懒加载目标,"\
        "对于输入节点而言, 这个插槽是唯一的目标插槽, 对于输出节点而言, 这个插槽是最后被连接的目标插槽",
                                           default=None, exclude=True)
    targetNodeID:       int        = Field(description="目标节点ID", default=-1)
    targetSlotName:     str        = Field(description="目标插槽名称", default="unknown")
    typeIndicator:      str        = Field(description="类型指示器, 此插槽的类型", default="unknown")
    IsInmappingSlot:    bool       = Field(description="是否为输入映射插槽", default=False)

    @virtual
    def __repr__(self) -> str:
        return f"{self.GetType().__name__}<name={self.slotName}, type={self.typeIndicator}, " \
               f"{'Input' if self.IsInmappingSlot else 'Output'}>"

    @virtual
    def TemplateClone(self) -> Self:
        return NodeSlotInfo(
            slotName=self.slotName,
            targetNodeID=self.targetNodeID,
            typeIndicator=self.typeIndicator,
            IsInmappingSlot=self.IsInmappingSlot,
        )
    @override
    def ToString(self) -> str:
        return self.slotName
    @override
    def SymbolName(self) -> str:
        return f"{self.GetType().__name__}<name={self.slotName}, type={self.typeIndicator}, " \
               f"{'Input' if self.IsInmappingSlot else 'Output'}>"

    @override
    def __init__(self, slotName:str, typeIndicator:str, IsInmappingSlot:bool, **kwargs:Any) -> Self:...
    def __init__(self, **kwargs:Any) -> Self:
        super().__init__(**kwargs)

class NodeInfo(BaseModel, any_class):
    """
    节点信息
    """
    node:           'Node'                  = Field(description="节点, 此变量需要手动同步, nodeID的懒加载目标",
                                                    default=None, exclude=True)
    nodeID:         int                     = Field(description="节点ID", default=-1)
    typename:       str                     = Field(description="节点类型", default="unknown")
    title:          str                     = Field(description="节点标题", default="unknown")
    inmapping:      Dict[str, NodeSlotInfo] = Field(description="输入映射", default={})
    outmapping:     Dict[str, NodeSlotInfo] = Field(description="输出映射", default={})
    position:       Vector2                 = Field(description="节点位置", default=Vector2(0, 0))

    @overload
    def __init__(
        self,
        *,
        nodeID:     Optional[int]                       = None,
        title:      Optional[str]                       = None,
        inmapping:  Optional[Dict[str, NodeSlotInfo]]   = None,
        outmapping: Optional[Dict[str, NodeSlotInfo]]   = None,
        **kwargs:Any
    ): ...
    def __init__(self, **kwargs:Any):
        if "title" not in kwargs:
            kwargs["title"] = kwargs.get("typename", self.GetType().__name__)
        if "typename" not in kwargs:
            kwargs["typename"] = self.GetType().__name__
        else:
            raise ValueError(f"节点的typename不能被指定")
        BaseModel.__init__(self, **kwargs)
        any_class.__init__(self)

    @virtual
    def __repr__(self) -> str:
        return f"{self.GetType().__name__}<typename={self.typename}, title={self.title}>"

    @virtual
    def TemplateClone(self) -> Self:
        result:NodeInfo = self.__class__(
            nodeID=self.nodeID,
            typename=self.typename,
            title=self.title,
            position=self.position
            )
        for key, value in self.inmapping.items():
            result.inmapping[key] = value.TemplateClone()
        for key, value in self.outmapping.items():
            result.outmapping[key] = value.TemplateClone()
        return result
    @virtual
    def CopyFromNode(self, node:'Node') -> None:
        self.nodeID = _Internal_GetNodeID(node)
        for key, inslot in node.Internal_Inmapping.items():
            self.inmapping[key] = inslot.info
        for key, outslot in node.Internal_Outmapping.items():
            self.outmapping[key] = outslot.info
    @override
    def ToString(self) -> str:
        return self.title
    @override
    def SymbolName(self) -> str:
        return f"{self.GetType().__name__}<typename={self.typename}, title={self.title}>"
    @virtual
    def Instantiate(self) -> 'Node':
        raise NotImplementedError(f"节点类型<{self.GetType().__name__}, typename={self.typename}"\
            f", trackback={self.generate_trackback}>未实现Instantiate方法")

class WorkflowEvent(BaseModel, any_class):
    @override
    def SymbolName(self) -> str:
        return self.GetType().__name__

class WorkflowStartEvent(WorkflowEvent):
    @override
    def SymbolName(self) -> str:
        return "StartEvent"

    verbose:bool = Field(description="是否为详细模式", default=False)

class WorkflowStopEvent(WorkflowEvent):
    @override
    def SymbolName(self) -> str:
        return "StopEvent"

    verbose:bool = Field(description="是否为详细模式", default=False)

class WorkflowErrorEvent(WorkflowStopEvent):
    @override
    def SymbolName(self) -> str:
        return "ErrorEvent"

    error:  Any   = Field(description="错误")
    from_:  Any   = Field(description="错误来源")

class _DefaultOptional(any_class):
    __instance:Optional[Self] = None

    def __new__(cls) -> Self:
        if cls.__instance is None:
            cls.__instance = super().__new__(cls)
        return cls.__instance

    @override
    def SymbolName(self) -> str:
        return "DefaultOptional"

    @staticmethod
    def GetInstance() -> Self:
        if _DefaultOptional.__instance is None:
            _DefaultOptional.__instance = _DefaultOptional()
        return _DefaultOptional.__instance

    @staticmethod
    def IsInstance(value:Any) -> bool:
        return isinstance(value, _DefaultOptional)

class NodeSlot(left_value_reference[NodeSlotInfo], BaseBehavior):
    """
    节点插槽
    """
    @property
    def info(self) -> NodeSlotInfo:
        return self.ref_value

    @classmethod
    def __get_pydantic_core_schema__(
        cls,
        _source_type: Any,
        _handler: GetCoreSchemaHandler,
    ) -> core_schema.CoreSchema:
        return core_schema.no_info_after_validator_function(
            cls,
            core_schema.any_schema(),
            serialization=core_schema.plain_serializer_function_ser_schema(
                lambda instance: None
            ),
        )

    __IsDirty:bool = False
    def SetDirty(self) -> None:
        self.__IsDirty = True

    @virtual
    def LinkVerify(self, other:Self) -> None:
        if self.info.IsInmappingSlot==other.info.IsInmappingSlot:
            raise ValueError(f"相同映射的插槽<{self.info.slotName}>和<{other.info.slotName}>不能连接")
        if self.info.typeIndicator!=other.info.typeIndicator:
            if not (
                    (self.info.typeIndicator == "string" and other.info.typeIndicator == "str") or
                    (self.info.typeIndicator == "str" and other.info.typeIndicator == "string") or
                    self.info.typeIndicator == Any or
                    other.info.typeIndicator == Any
                ):
                raise ValueError(f"类型不匹配的插槽<{self.info.slotName}>和<{other.info.slotName}>不能连接")
        if self.info.parentNode==other.info.parentNode:
            raise ValueError(f"父节点相同的插槽<node={self.info.parentNode.SymbolName()}, id={_Internal_GetNodeID(self.info.parentNode)}>"\
                f"<name={self.info.slotName}, type={self.info.typeIndicator}>和"\
                f"<name={other.info.slotName}, type={other.info.typeIndicator}>不能连接")

    @classmethod
    def Link(cls, left:Self, right:Self) -> None:
        left.LinkVerify(right)
        if left.info.IsInmappingSlot or left.info.targetSlot == right:
            cls.Unlink(left)
        left.info.targetSlot = right
        left.info.targetSlotName = right.info.slotName
        left.info.targetNode = right.info.parentNode
        left.info.targetNodeID = _Internal_GetNodeID(right.info.targetNode)
        left.SetDirty()
        if right.info.IsInmappingSlot or right.info.targetSlot == left:
            cls.Unlink(right)
        right.info.targetSlot = left
        right.info.targetSlotName = left.info.slotName
        right.info.targetNode = left.info.parentNode
        right.info.targetNodeID = _Internal_GetNodeID(left.info.targetNode)
        right.SetDirty()
    @classmethod
    def Unlink(cls, slot:Self) -> None:
        targetSlot:Optional[NodeSlot] = slot.info.targetSlot
        slot.info.targetSlot = None
        slot.info.targetNode = None
        slot.info.targetNodeID = -1
        # 输出槽存在多连接, 因此如果不统一的话可以不被断连
        if targetSlot is not None and targetSlot.info.targetSlot == slot:
            targetSlot.info.targetSlot = None
            targetSlot.info.targetNode = None
            targetSlot.info.targetNodeID = -1
            targetSlot.SetDirty()
        slot.SetDirty()

    def LinkTo(self, other:Optional[Self]) -> None:
        if other is None:
            NodeSlot.Unlink(self)
        else:
            NodeSlot.Link(self, other)

    def SetupFromInfo(self, info:NodeSlotInfo) -> None:
        if info != self.info:
            self.info = info
            info.slot = self
            self.SetDirty()

    def UpdateImmediate(self) -> None:
        self.__IsDirty = False

    @override
    def OnUpdate(self) -> None:
        if self.__IsDirty:
            self.UpdateImmediate()

    def __init__(
        self,
        info:   NodeSlotInfo,
        *,
        parent: Optional['Node']                    = None,
        target: Optional[Tuple['Node', 'NodeSlot']] = None,
        ):
        super().__init__(info)
        if parent is not None:
            self.info.parentNode = parent
        if target is not None:
            self.info.targetNode = target[0]
            self.info.targetSlot = target[1]

    @override
    def ToString(self) -> str:
        return f"{self.GetType().__name__}<{self.info}>"
    @override
    def SymbolName(self) -> str:
        return f"{self.GetType().__name__}<name={self.info.slotName}, type={self.info.typeIndicator}, " \
               f"parent={self.info.parentNode.SymbolName()}>"

    __is_start:bool = False
    @sealed
    def OnStartEvent(self, event:Any) -> None:
        if isinstance(event, WorkflowStartEvent):
            self.__is_start = True
    @sealed
    def OnStopEvent(self, event:Any) -> None:
        if isinstance(event, WorkflowStopEvent):
            self.__is_start = False
            self.ClearParameter()

    __parameter:left_value_reference[context_value_type] = None
    '''
    类型是info.typeIndicator指示的类型, 左值包装的上下文参数
    '''
    @sealed
    async def GetParameter(self) -> context_value_type:
        '''
        异步函数, 获取参数,
        输出槽:
            当参数还未被节点设置时触发节点, 等待擦参数被设置
        输入槽:
            获取上游输出槽的参数, 当上游参数还未被设置时应等待
        '''
        if self.info.IsInmappingSlot:
            if self.info.targetSlot is None:
                return _DefaultOptional.GetInstance()
            if GetInternalWorkflowDebug():
                print_colorful(ConsoleFrontColor.YELLOW, f"节点<{self.info.parentNode.SymbolName()}, "\
                    f"id={_Internal_GetNodeID(self.info.parentNode)}, title={self.info.parentNode.info.title}>"\
                    f"的输入槽{ConsoleFrontColor.RESET}<{self.info.slotName}>{ConsoleFrontColor.YELLOW}"\
                    f"从上游输出槽{ConsoleFrontColor.RESET}<{self.info.targetSlotName}>{ConsoleFrontColor.YELLOW}获取参数")
            return await self.info.targetSlot.GetParameter()
        else:
            if GetInternalWorkflowDebug():
                print_colorful(ConsoleFrontColor.YELLOW, f"节点<{self.info.parentNode.SymbolName()}, "\
                    f"id={_Internal_GetNodeID(self.info.parentNode)}, title={self.info.parentNode.info.title}>"\
                    f"的输出槽{ConsoleFrontColor.RESET}<{self.info.slotName}>{ConsoleFrontColor.YELLOW}"\
                    f"等待父节点<type={self.info.parentNode.SymbolName()}, "\
                    f"id={_Internal_GetNodeID(self.info.parentNode)}, "\
                    f"title={self.info.parentNode.info.title}>执行Step")
            await self.info.parentNode.RunStep()
            while self.__parameter is None and self.__is_start:
                await asyncio.sleep(0.1)
            if self.__is_start:
                if GetInternalWorkflowDebug():
                    print_colorful(ConsoleFrontColor.YELLOW, f"节点<{self.info.parentNode.SymbolName()}, "\
                        f"id={_Internal_GetNodeID(self.info.parentNode)}, title={self.info.parentNode.info.title}>"\
                        f"的输出槽{ConsoleFrontColor.RESET}<{self.info.slotName}>{ConsoleFrontColor.YELLOW}"\
                        f"输出为{ConsoleFrontColor.GREEN}{limit_str(self.__parameter.ref_value, 100)}{ConsoleFrontColor.YELLOW}")
                return self.__parameter.ref_value
            else:
                if GetInternalWorkflowDebug():
                    print_colorful(ConsoleFrontColor.YELLOW, f"节点<{self.info.parentNode.SymbolName()}, "\
                        f"id={_Internal_GetNodeID(self.info.parentNode)}, title={self.info.parentNode.info.title}>"\
                        f"的输出槽{ConsoleFrontColor.RESET}<{self.info.slotName}>{ConsoleFrontColor.YELLOW}"\
                        f"任务未开始, 任务取消")
                return None
    @sealed
    def SetParameter(self, value:Optional[context_value_type]) -> None:
        '''
        设置参数
        '''
        if self.info.typeIndicator == "NoneType" and value is not None:
            raise ValueError(f"该插槽<{self.info.slotName}>的类型为NoneType, 不能设置参数")
        if self.__parameter is None:
            self.__parameter = left_value_reference[context_value_type](value)
        else:
            self.__parameter.ref_value = value
    @sealed
    def ClearParameter(self) -> None:
        '''
        清除参数, 接收到StopEvent后调用
        '''
        self.__parameter = None

    @override
    def __repr__(self) -> str:
        return f"NodeSlot<{self.info.slotName}, id={_Internal_GetNodeID(self.info.parentNode)}>"

class Node(left_value_reference[NodeInfo], BaseBehavior):
    """
    节点
    """
    @property
    def IsRunning(self) -> bool:
        return self._is_start and self._results is None

    def __init__(self, info:NodeInfo) -> None:
        super().__init__(None)
        self._Internal_Inmapping:Dict[str, NodeSlot] = {}
        self._Internal_Outmapping:Dict[str, NodeSlot] = {}
        self.SetupFromInfo(info)

    @property
    def info(self) -> NodeInfo:
        return self.ref_value

    @classmethod
    def __get_pydantic_core_schema__(
        cls,
        _source_type: Any,
        _handler: GetCoreSchemaHandler,
    ) -> core_schema.CoreSchema:
        return core_schema.no_info_after_validator_function(
            cls,
            core_schema.any_schema(),
            serialization=core_schema.plain_serializer_function_ser_schema(
                lambda instance: None
            ),
        )

    _Internal_Inmapping:Dict[str, NodeSlot] = {}
    _Internal_Outmapping:Dict[str, NodeSlot] = {}
    @property
    def Internal_Inmapping(self) -> Dict[str, NodeSlot]:
        return self._Internal_Inmapping
    @property
    def Internal_Outmapping(self) -> Dict[str, NodeSlot]:
        return self._Internal_Outmapping

    @sealed
    def ClearLink(self) -> None:
        '''
        清除所有连接
        '''
        for slot in self.Internal_Inmapping.values():
            #slot.info.targetNode = None
            #slot.info.targetSlot = None
            NodeSlot.Unlink(slot)
            slot.SetDirty()
        for slot in self.Internal_Outmapping.values():
            #slot.info.targetNode = None
            #slot.info.targetSlot = None
            NodeSlot.Unlink(slot)
            slot.SetDirty()
    @sealed
    def ClearSlots(self) -> None:
        '''
        清空所有槽的实例
        '''
        if self.info is None:
            return
        if GetInternalWorkflowDebug():
            print_colorful(ConsoleFrontColor.BLUE, f"{self.SymbolName()}"\
                f"<id={_Internal_GetNodeID(self)}, title={self.info.title if self.info is not None else '<no info>'}>ClearLink")
        self.Internal_Inmapping.clear()
        self.Internal_Outmapping.clear()
    @sealed
    def BuildSlots(self) -> None:
        '''
        从info中构建槽的实例
        '''
        if self.info is None:
            raise ValueError(f"节点<{self.SymbolName()}>未设置info")
        if GetInternalWorkflowDebug():
            print_colorful(ConsoleFrontColor.BLUE, f"{self.SymbolName()}"\
                f"<id={_Internal_GetNodeID(self)}, title={self.info.title}>BuildLink")
        for slot_name, info in self.info.inmapping.items():
            if GetInternalWorkflowDebug():
                print_colorful(ConsoleFrontColor.BLUE, f"{self.SymbolName()}"\
                    f"<id={_Internal_GetNodeID(self)}, title={self.info.title}>"\
                    f"在<{slot_name}>处创建输入槽")
            self.Internal_Inmapping[slot_name] = NodeSlot(info.TemplateClone(), parent=self)
        for slot_name, info in self.info.outmapping.items():
            if GetInternalWorkflowDebug():
                print_colorful(ConsoleFrontColor.BLUE, f"{self.SymbolName()}"\
                    f"<id={_Internal_GetNodeID(self)}, title={self.info.title}>"\
                    f"在<{slot_name}>处创建输出槽")
            self.Internal_Outmapping[slot_name] = NodeSlot(info.TemplateClone(), parent=self)
    @sealed
    def BuildLink(self,*,info:Optional[NodeInfo]=None) -> None:
        '''
        从指定或本身的info中构建连接
        '''
        if info is None:
            info = self.info
        for slot_name, slot_info in info.inmapping.items():
            targetNode = _Internal_GetNode(slot_info.targetNodeID)
            if targetNode is not None:
                NodeSlot.Link(self.Internal_Inmapping[slot_name], targetNode.Internal_Outmapping[slot_info.targetSlotName])
            else:
                NodeSlot.Unlink(self.Internal_Inmapping[slot_name])
        for slot_name, slot_info in info.outmapping.items():
            targetNode = _Internal_GetNode(slot_info.targetNodeID)
            if targetNode is not None:
                NodeSlot.Link(self.Internal_Outmapping[slot_name], targetNode.Internal_Inmapping[slot_info.targetSlotName])
            else:
                NodeSlot.Unlink(self.Internal_Outmapping[slot_name])
    @sealed
    def RefreshImmediate(self) -> None:
        pass

    def SetupFromInfo(self, info:NodeInfo) -> None:
        if GetInternalWorkflowDebug():
            if self.info is not None:
                print_colorful(ConsoleFrontColor.YELLOW, f"{self.SymbolName()}"\
                    f"<info={self.info.SymbolName()}, title={self.info.title}>"\
                    f"SetupFromInfo(info={info.SymbolName()}, title={info.title})")
            else:
                print_colorful(ConsoleFrontColor.YELLOW, f"{self.SymbolName()}"\
                    f"SetupFromInfo(info={info.SymbolName()}, title={info.title})")
        self.ClearSlots()
        self.ref_value = info
        if _Internal_GetNodeID(self) != -1:
            self.info.nodeID = _Internal_GetNodeID(self)
            self.info.node = self
            self.BuildSlots()
        else:
            self.info.node = self

    def link_inslot_to_other_node_outslot(self, other:Self, slotName:str, targetSlotName:str) -> None:
        NodeSlot.Link(self.Internal_Outmapping[slotName], other.Internal_Inmapping[targetSlotName])
    def link_outslot_to_other_node_inslot(self, other:Self, slotName:str, targetSlotName:str) -> None:
        NodeSlot.Link(self.Internal_Inmapping[slotName], other.Internal_Outmapping[targetSlotName])
    def unlink_inslot(self, slotName:str) -> None:
        NodeSlot.Unlink(self.Internal_Inmapping[slotName])
    def unlink_outslot(self, slotName:str) -> None:
        NodeSlot.Unlink(self.Internal_Outmapping[slotName])

    _is_start:bool = False
    @virtual
    def OnStartEvent(self, event:any_class) -> None:
        if isinstance(event, WorkflowStartEvent):
            self._is_start = True
            if event.verbose:
                print_colorful(ConsoleFrontColor.GREEN, f"{self.__class__.__name__}"\
                    f"<id={_Internal_GetNodeID(self)}> catch event: {event.GetType()}")
    @virtual
    def OnStopEvent(self, event:any_class) -> None:
        if isinstance(event, WorkflowStopEvent):
            self._is_start = False
            self.ClearResult()
            self.ClearParameters()
            if event.verbose:
                print_colorful(ConsoleFrontColor.GREEN, f"{self.__class__.__name__}"\
                    f"<id={_Internal_GetNodeID(self)}> catch event: {event.GetType()}")
    @property
    def is_start(self) -> bool:
        return self._is_start

    _parameters:Optional[Dict[str, context_value_type]] = None
    _results:Optional[Dict[str, context_value_type]]|context_value_type = None
    @sealed
    def GetResult(self) -> context_value_type|None:
        return self._results
    @sealed
    async def BuildParameter(self) -> None:
        '''
        异步函数, 从inslots中构建参数
        '''
        parameters = {}
        if GetInternalWorkflowDebug():
            print_colorful(ConsoleFrontColor.YELLOW, f"{self.SymbolName()}"\
                f"<id={_Internal_GetNodeID(self)}, title={self.info.title}>拥有输入槽:{ConsoleFrontColor.WHITE}"\
                f"{self.Internal_Inmapping.keys()}{ConsoleFrontColor.YELLOW}")
        for slot_name, slot in self.Internal_Inmapping.items():
            if GetInternalWorkflowDebug():
                print_colorful(ConsoleFrontColor.YELLOW, f"{self.SymbolName()}"\
                    f"<id={_Internal_GetNodeID(self)}, title={self.info.title}>位于"\
                    f"{ConsoleFrontColor.RESET}<{slot_name}>{ConsoleFrontColor.YELLOW}的槽开始获取参数")
            value = await slot.GetParameter()
            if not _DefaultOptional.IsInstance(value):
                parameters[slot_name] = value
        if GetInternalWorkflowDebug():
            print_colorful(ConsoleFrontColor.YELLOW, f"{self.SymbolName()}"\
                f"<id={_Internal_GetNodeID(self)}, title={self.info.title}>参数构建完成:{ConsoleFrontColor.WHITE}"\
                f"{limit_str(self._parameters, 100)}{ConsoleFrontColor.YELLOW}")
        self._parameters = parameters
    @sealed
    async def GetParameters(self) -> Dict[str, context_value_type]:
        if not self._is_start:
            if GetInternalWorkflowDebug():
                print_colorful(ConsoleFrontColor.YELLOW, f"{self.SymbolName()}"\
                    f"<id={_Internal_GetNodeID(self)}, title={self.info.title}>任务未开始")
            return {}
        if self._parameters is None:
            if GetInternalWorkflowDebug():
                print_colorful(ConsoleFrontColor.YELLOW, f"{self.SymbolName()}"\
                    f"<id={_Internal_GetNodeID(self)}, title={self.info.title}>开始构建参数")
            await self.BuildParameter()
        return self._parameters
    @sealed
    def ClearParameters(self) -> None:
        self._parameters = None
    @sealed
    def ClearResult(self) -> None:
        self._results = None
    @sealed
    async def RunStep(self) -> None:
        if self._results is not None:
            if GetInternalWorkflowDebug():
                print_colorful(ConsoleFrontColor.YELLOW, f"{self.SymbolName()}"\
                    f"<id={_Internal_GetNodeID(self)}, title={self.info.title}>结果"\
                    f"<{limit_str(self._results, 100)}>已存在")
            return
        try:
            if GetInternalWorkflowDebug():
                print_colorful(ConsoleFrontColor.YELLOW, f"{self.SymbolName()}"\
                    f"<id={_Internal_GetNodeID(self)}, title={self.info.title}>开始执行")
            # 使用本地属性__parameters接收了参数
            await self.GetParameters()
            self._results = await self._DoRunStep()
            if isinstance(self._results, dict):
                for key, value in self._results.items():
                    self.Internal_Outmapping[key].SetParameter(value)
            else:
                if len(self.Internal_Outmapping.items()) == 1:
                    self.Internal_Outmapping[next(iter(self.Internal_Outmapping.keys()))].SetParameter(self._results)
                elif len(self.Internal_Outmapping.items()) == 0:
                    pass
                else:
                    self.Internal_Outmapping["result"].SetParameter(self._results)
        except Exception as e:
            #self.Broadcast(WorkflowErrorEvent(error=e, from_=self), "OnStopEvent")
            raise

    @virtual
    async def _DoRunStep(self) -> Dict[str, context_value_type]|context_value_type:
        raise NotImplementedError(f"节点类型<{self.__class__.__name__}>未实现_DoRunStep方法")

    @override
    def __repr__(self) -> str:
        return f"Node<{self.info.title}, id={_Internal_GetNodeID(self)}>"

    @override
    def SymbolName(self) -> str:
        return self.GetType().__name__

class FunctionModel(BaseModel, any_class):
    """
    函数模型
    """
    name:           str             = Field(description="函数名称")
    description:    Optional[str]   = Field(description="函数描述")
    parameters:     Dict[str, str]  = Field(description="函数参数")
    returns:        Dict[str, str]  = Field(description="函数返回值")

class Workflow(BaseModel, any_class):
    """
    工作流信息
    """
    Datas:        List[NodeInfo] = Field(description="节点信息", default=[])
    Nodes:        List[Node]     = Field(description="节点, 此变量需要手动同步, nodeID的懒加载目标",
                                         default=[], exclude=True)
    Functions:    List[FunctionModel] = Field(description="函数模型", default=[])

    @classmethod
    def CreateTemplate(cls, one_end:Optional['EndNodeInfo']) -> Self:
        if one_end is None:
            one_end = EndNodeInfo()
        return cls(
            Datas=[
                one_end
            ]
        )
    @classmethod
    def Create(cls, *args:NodeInfo|List[NodeInfo], is_auto_id:bool=True) -> Self:
        workflow = cls(
            Datas=to_list(args)
        )
        if is_auto_id:
            index = 0
            for info in workflow.Datas:
                info.nodeID = index
                index += 1
        return workflow

__all__workflow_action_wrappers__:Dict[action_label_type, 'WorkflowActionWrapper'] = {}

class WorkflowActionWrapper(left_value_reference[Callable], invoke_callable):
    name:           action_label_type   = None
    functionModel:  FunctionModel       = None

    def __init__(
        self,
        name:           action_label_type,
        action:         Callable,
        description:    Optional[str]               = None,
        parameters:     Optional[Dict[str, str]]    = None,
        returns:        Optional[Dict[str, str]]    = None,
        ):
        super().__init__(action)
        self.name = name
        if parameters is None or returns is None:
            methodInfo = MethodInfo.Create(name, action)
            if parameters is None:
                parameters = {param.ParameterName: str(param.ParameterType) for param in methodInfo.Parameters}
            if returns is None:
                returns = {"result": str(methodInfo.ReturnType)}
        if parameters is not None and returns is not None:
            self.functionModel = FunctionModel(
                name=name,
                description=description or action.__doc__ or "",
                parameters=parameters,
                returns=returns
            )
        if name in __all__workflow_action_wrappers__:
            if GetInternalWorkflowDebug():
                print_colorful(ConsoleFrontColor.YELLOW, f"警告: 工作流动作<{name}>已存在, 将覆盖旧的ActionWrapper")
            __all__workflow_action_wrappers__[name] = None
        __all__workflow_action_wrappers__[name] = self

    def __del__(self):
        if self.name in __all__workflow_action_wrappers__:
            del __all__workflow_action_wrappers__[self.name]

    def __call__(self, *args: Any, **kwds: Any) -> Any:
        return self.ref_value(*args, **kwds)

    @classmethod
    def GetAllActionWrappers(cls) -> Dict[str, Self]:
        return __all__workflow_action_wrappers__

    @classmethod
    def GetActionWrapper(cls, name:action_label_type) -> Self:
        return __all__workflow_action_wrappers__[name]

    @classmethod
    def GetActionWrapperNames(cls) -> List[action_label_type]:
        return list(__all__workflow_action_wrappers__.keys())

    @classmethod
    def ContainsActionWrapper(cls, name:action_label_type) -> bool:
        return name in __all__workflow_action_wrappers__

    @classmethod
    def GetActionWrapperModels(cls) -> List[FunctionModel]:
        return [wrapper.functionModel for wrapper in __all__workflow_action_wrappers__.values()
                if wrapper is not None and wrapper.functionModel is not None]

_Internal_All_EndNodes:List['EndNode'] = []
_Internal_Task_Count:atomic[int] = atomic(0, threading.Lock())

class NodeResult(BaseModel, any_class):
    """
    工作流结果
    """
    nodeID:     int                             = Field(description="节点ID")
    nodeTitle:  str                             = Field(description="节点标题")
    result:     context_type|context_value_type = Field(description="节点结果", default={})

    @override
    def ToString(self) -> str:
        return f"{self.nodeTitle}<{self.nodeID}>: {self.result}"
    def __str__(self) -> str:
        return self.ToString()

class ContextResult(BaseModel, any_class):
    """
    上下文结果
    """
    hashID:     str              = Field(description="结果哈希", default="")
    results:    List[NodeResult] = Field(description="节点结果", default=[])
    progress:   float            = Field(description="进度", default=0.0)
    task_count: int              = Field(description="任务数量", default=0)

    @override
    def ToString(self) -> str:
        return f"[{', '.join([Unwrapper2Str(result) for result in self.results])}]<progress={self.progress}, task_count={self.task_count}>"
    def __str__(self) -> str:
        return self.ToString()

class WorkflowManager(left_value_reference[Workflow], BaseBehavior):
    """
    工作流管理器
    """
    def __init__(self, workflow:Workflow):
        left_value_reference[Workflow].__init__(self, workflow)
        BaseBehavior.__init__(self)
        global _workflow_manager_instance
        if _workflow_manager_instance is not None:
            raise Exception("WorkflowManager 只能有一个实例")
        _workflow_manager_instance = self
        self.__state = WorkflowState()
        self.__timeout = 300  # 默认5分钟超时

    __callbackDatas:List[Tuple[str, Callable[[dict], None]]] = []

    def CreateNode(self, info:NodeInfo) -> Node:
        if GetInternalWorkflowDebug():
            print_colorful(ConsoleFrontColor.YELLOW, f"节点{info.title}<{info.__class__.__name__}>创建")
        node:Node = info.Instantiate()
        self.workflow.Nodes.append(node)
        node.BuildSlots()
        return node

    def DestroyNode(self, node:Node) -> None:
        self.workflow.Nodes.remove(node)

    def SetupWorkflowNodeType(self, label:str, template:NodeInfo) -> None:
        def closure(**kwargs:Any) -> None:
            info = template.TemplateClone()
            for key, value in kwargs.items():
                if hasattr(info, key):
                    setattr(info, key, value)
            node = self.CreateNode(info)
        self.__callbackDatas.append((label, closure))

    @property
    def workflow(self) -> Workflow:
        return self.ref_value

    @classmethod
    def GetInstance(cls) -> Self:
        global _workflow_manager_instance
        if _workflow_manager_instance is None:
            _workflow_manager_instance = WorkflowManager(None)
        return _workflow_manager_instance

    def ClearWorkflow(self) -> Self:
        if self.ref_value is not None:
            self.ref_value.Datas.clear()
            self.ref_value.Nodes.clear()
        return self

    def BuildWorkflow(self) -> Self:
        """构建工作流"""
        try:
            for info in self.ref_value.Datas:
                self.CreateNode(info)
            WorkflowValidator.validate_workflow(self.workflow)
            return self
        except WorkflowValidationError as e:
            self.__state.error = str(e)
            raise

    def RefreshImmediate(self) -> Self:
        for node in self.ref_value.Nodes:
            node.RefreshImmediate()
        return None

    def ContainsNode(self, id:int) -> bool:
        if id < 0:
            return False
        return any(node.nodeID == id for node in self.ref_value.Datas)

    def GetNode(self, id:int) -> Node:
        if id < 0:
            return None
        return self.workflow.Nodes[id]

    def GetNodeInfo(self, id:int) -> NodeInfo:
        if id < 0:
            return None
        return self.workflow.Datas[id]

    def GetNodeID(self, node:Optional[Node]) -> int:
        if node is None:
            return -1
        if node not in self.workflow.Nodes:
            return -1
        return self.workflow.Nodes.index(node)

    def SaveWorkflow(self, file:tool_file_or_str) -> tool_file:
        file = Wrapper2File(file)
        if file.dirpath is not None and not Wrapper2File(file.dirpath).exists():
            raise Exception(f"{file.dirpath} 不存在")
        currentWorkflow:Workflow = self.workflow
        currentWorkflow.Datas.clear()
        for node in currentWorkflow.Nodes:
            node.info.CopyFromNode(node)
            currentWorkflow.Datas.append(node.info)
        return EasySave.Write(currentWorkflow, file)

    def LoadWorkflow(self, workflow_:tool_file_or_str|Workflow) -> Workflow:
        workflow:Workflow = None
        if isinstance(workflow_, Workflow):
            self.ClearWorkflow()
            workflow = workflow_
        else:
            workflow_ = Wrapper2File(workflow_)
            if not workflow_.exists():
                raise Exception(f"{workflow_} 不存在")
            workflow:Workflow = EasySave.Read(Workflow, workflow_)
            self.ClearWorkflow()
        # 排序及检查
        workflow.Datas.sort(key=lambda x: x.nodeID)
        for i, info in enumerate(workflow.Datas):
            if info.nodeID != i:
                raise ValueError(f"异常的工作流: 索引与节点ID不匹配")
        # 以复制的形式新建节点
        self.ref_value = Workflow()
        for info in workflow.Datas:
            node = self.CreateNode(info)
        # 新建节点后手动将其连接
        for node in self.workflow.Nodes:
            node.BuildLink()
        return self.workflow

    @property
    def state(self) -> 'WorkflowState':
        return self.__state

    def set_timeout(self, seconds: float) -> None:
        """设置工作流超时时间"""
        self.__timeout = seconds

    async def _DoStart(self, verbose:bool=False) -> None:
        '''
        开始工作流
        '''
        global _Internal_All_EndNodes
        if len(_Internal_All_EndNodes) == 0:
            if GetInternalWorkflowDebug():
                print_colorful(ConsoleFrontColor.YELLOW, "工作流为空")
            return

        global _Internal_Task_Count
        _Internal_Task_Count.store(len(_Internal_All_EndNodes))

        if GetInternalWorkflowDebug():
            print_colorful(ConsoleFrontColor.YELLOW, f"工作流开始, 任务数量: {_Internal_Task_Count}, 开始时间: {time.time()}")

        # 广播开始事件
        self.Broadcast(WorkflowStartEvent(verbose=verbose), "OnStartEvent")

        try:
            # 等待所有终止节点完成
            while _Internal_Task_Count.load() > 0:
                await asyncio.sleep(0.1)

            # 所有任务正常完成
            self.__state.is_completed = True

        except Exception as e:
            # 发生异常，确保停止工作流
            if GetInternalWorkflowDebug():
                print_colorful(ConsoleFrontColor.RED, f"工作流执行过程中发生错误: {e}")

            # 停止工作流并传递错误信息
            self.StopWorkflow(error=e)

            # 重新抛出异常
            raise
        finally:
            # 确保总是广播停止事件
            if GetInternalWorkflowDebug():
                print_colorful(ConsoleFrontColor.YELLOW, f"工作流结束, 结束时间: {time.time()}")

            self.Broadcast(WorkflowStopEvent(verbose=verbose), "OnStopEvent")

    async def RunWorkflow(self, verbose:bool=False) -> None:
        """运行工作流"""
        if verbose:
            print_colorful(ConsoleFrontColor.BLUE, "运行工作流")
        try:
            self.__state = WorkflowState()
            self.__state.start_time = time.time()

            if GetInternalWorkflowDebug():
                print_colorful(ConsoleFrontColor.YELLOW, f"_DoStart尝试启动")
            # 启动工作流
            await self._DoStart(verbose)
            if GetInternalWorkflowDebug():
                print_colorful(ConsoleFrontColor.YELLOW, f"_DoStart完成")

            # 等待完成或超时
            while not self.__state.is_completed:
                if time.time() - self.__state.start_time > self.__timeout:
                    self.StopWorkflow()
                    raise WorkflowTimeoutError(f"工作流执行超时: {self.__timeout}秒")
                await asyncio.sleep(0.1)

        except Exception as e:
            self.__state.error = str(e)
            # 确保在发生异常时停止工作流，并传递错误信息
            #self.StopWorkflow(error=e)
            #self.__state.end_time = time.time()
            # 确保广播停止事件不需要再次调用，因为StopWorkflow已经处理
            # self.Broadcast(WorkflowStopEvent(verbose=verbose), "OnStopEvent")
            raise
        finally:
            # 无论是否发生异常，都确保工作流状态正确
            self.__state.end_time = time.time()
            self.__state.is_completed = True
            # 确保广播停止事件
            self.Broadcast(WorkflowStopEvent(verbose=verbose), "OnStopEvent")

            if verbose:
                print_colorful(ConsoleFrontColor.BLUE, f"工作流完成, 用时: "\
                    f"{time.time() - self.__state.start_time}秒, 异常状态: {self.__state.error}")

    def StopWorkflow(self, file: Optional[tool_file_or_str]=None, error: Optional[Exception]=None) -> None:
        '''
        打断任务, 直接停止工作流

        Args:
            file: 保存工作流状态的文件
            error: 停止工作流的错误原因
        '''
        # if file is not None:
        #     self.SaveState(file)
        global _Internal_Task_Count

        # 如果提供了错误信息，广播错误事件
        if error is not None:
            self.__state.error = str(error)
            self.Broadcast(WorkflowErrorEvent(verbose=True, error=error, from_=self), "OnStopEvent")

        # 确保任务计数归零，以便_DoStart中的循环能够退出
        _Internal_Task_Count.store(0)

        # 记录结束时间
        if self.__state.end_time is None:
            self.__state.end_time = time.time()

        # 标记工作流已完成
        self.__state.is_completed = True

    @override
    def __repr__(self) -> str:
        return f"{self.__class__.__name__}"

    def GetCurrentContext(self) -> ContextResult:
        """获取当前上下文, 也就是当前的结果"""
        result:ContextResult = ContextResult()
        node_running_count = 0
        for index, node in enumerate(self.workflow.Nodes):
            if isinstance(node, EndNode):
                result.results.append(NodeResult(nodeID=index, nodeTitle=node.info.title, result=node.end_result))
            elif node.IsRunning:
                node_running_count += 1
        result.progress = 1 - (node_running_count / len(self.workflow.Nodes))
        result.task_count = _Internal_Task_Count.load()
        import hashlib
        result.hashID = hashlib.md5(result.ToString().encode()).hexdigest()
        return result

class DynamicNode(Node):
    """
    动态节点, 在工作流中动态创建
    """
    __action_name:action_label_type = None

    def __init__(self, info:NodeInfo, action:action_label_type) -> None:
        super().__init__(info)
        self.__action_name = action

    @override
    async def _DoRunStep(self) -> Dict[str, context_value_type]|context_value_type:
        func = WorkflowActionWrapper.GetActionWrapper(self.__action_name)
        return func(**self.GetParameters())

    def add_slot(self, name:str, typeIndicator:str|type, IsInmappingSlot:bool=True) -> bool:
        if name in self.info.inmapping:
            return False
        if IsInmappingSlot:
            self.info.inmapping[name] = NodeSlotInfo(
                parentNode=self,
                slotName=name,
                typeIndicator=f"{typeIndicator}",
                IsInmappingSlot=IsInmappingSlot
            )
        else:
            self.info.outmapping[name] = NodeSlotInfo(
                parentNode=self,
                slotName=name,
                typeIndicator=f"{typeIndicator}",
                IsInmappingSlot=IsInmappingSlot
            )
        return True
    def delete_slot(self, name:str, IsInmappingSlot:bool=True) -> bool:
        if IsInmappingSlot:
            if name not in self.info.inmapping:
                return False
            del self.info.inmapping[name]
        else:
            if name not in self.info.outmapping:
                return False
            del self.info.outmapping[name]
        return True

class DynamicNodeInfo(NodeInfo):
    """
    动态节点信息, 属于动态节点, 用于动态创建节点
    """

    def Instantiate(self) -> Node:
        return DynamicNode(self)

class StartNode(Node):
    """
    开始节点, 工作流的起点
    """
    def __init__(self, info:NodeInfo) -> None:
        super().__init__(info)

    @override
    async def _DoRunStep(self) -> Dict[str, context_value_type]|context_value_type:
        raise NotImplementedError(f"开始节点<{self.__class__.__name__}>未实现_DoRunStep方法")

class StartNodeInfo(NodeInfo):
    """
    开始节点信息, 属于开始节点, 用于开始工作流
    """

    @override
    def Instantiate(self) -> Node:
        raise NotImplementedError(f"开始节点<{self.__class__.__name__}>未实现Instantiate方法")

class StepNode(Node):
    """
    步骤节点, 在工作流中触发
    """
    def __init__(self, info:NodeInfo) -> None:
        super().__init__(info)

    @override
    @property
    def info(self) -> 'StepNodeInfo':
        return self.ref_value

    _verbose:bool = False

    @override
    def OnStartEvent(self, event:Any) -> None:
        super().OnStartEvent(event)
        if isinstance(event, WorkflowStartEvent):
            self._verbose = event.verbose
    @override
    async def _DoRunStep(self) -> Dict[str, context_value_type]|context_value_type:
        func = WorkflowActionWrapper.GetActionWrapper(self.info.funcname)
        if self._verbose:
            print(f"{self.__class__.__name__}<id={_Internal_GetNodeID(self)}> start func: {self.info.funcname}")
        try:
            current_parameters = await self.GetParameters()
            coroutine_or_result = func(**current_parameters)
            result:Any = None
            if isinstance(coroutine_or_result, Coroutine):
                result = await coroutine_or_result
            else:
                result = coroutine_or_result
            if self._verbose:
                print(f"{self.__class__.__name__}<id={_Internal_GetNodeID(self)}> "\
                    f"end func: {self.info.funcname}, result: {ConsoleFrontColor.GREEN}{limit_str(result, 100)}{ConsoleFrontColor.YELLOW}")
            return result
        except Exception as e:
            if self._verbose:
                print(f"{self.__class__.__name__}<id={_Internal_GetNodeID(self)}> error in func: {self.info.funcname}, error: {e}")
            raise

class StepNodeInfo(NodeInfo):
    """
    步骤节点, 在工作流中触发
    """
    funcname:action_label_type = Field(description="函数键名", default="unknown")
    @override
    def Instantiate(self) -> Node:
        return StepNode(self)

class EndNode(Node):
    """
    结束节点, 工作流的终端
    """
    def __init__(self, info:NodeInfo) -> None:
        super().__init__(info)
        global _Internal_All_EndNodes
        _Internal_All_EndNodes.append(self)
    def __del__(self):
        _Internal_All_EndNodes.remove(self)

    end_result:Optional[context_value_type|Dict[str, context_value_type]] = None

    @override
    async def _DoRunStep(self) -> Dict[str, context_value_type]|context_value_type:
        if GetInternalWorkflowDebug():
            print_colorful(ConsoleFrontColor.YELLOW, f"{self.SymbolName()}"\
                f"<id={_Internal_GetNodeID(self)}>结束节点开始获取结果")
        self.end_result:Dict[str, context_value_type] = await self.GetParameters()
        if len(self.end_result) == 1:
            self.end_result = next(iter(self.end_result.values()))
        elif len(self.end_result) == 0:
            self.end_result = None
        return self.end_result

    async def _BeginGetParameters(self) -> None:
        global _Internal_Task_Count
        if GetInternalWorkflowDebug():
            print_colorful(ConsoleFrontColor.YELLOW, f"{self.SymbolName()}"\
                f"<id={_Internal_GetNodeID(self)}>开始获取结果, 当前任务数量: {_Internal_Task_Count}")
        await self.RunStep()
        self._is_start = False
        _Internal_Task_Count.fetch_sub(1)
        if GetInternalWorkflowDebug():
            print_colorful(ConsoleFrontColor.YELLOW, f"{self.SymbolName()}"\
                f"<id={_Internal_GetNodeID(self)}>任务结束, 当前任务数量: {_Internal_Task_Count}")

    @override
    def OnLateUpdate(self) -> None:
        super().OnLateUpdate()
        if self.is_start:
            run_async_coroutine(self._BeginGetParameters())

    def add_slot(self, name:str, typeIndicator:str|type) -> bool:
        if name in self.info.inmapping:
            return False
        self.info.inmapping[name] = NodeSlotInfo(
            parentNode=self,
            slotName=name,
            typeIndicator=f"{typeIndicator}",
            IsInmappingSlot=True
        )
        self.Internal_Inmapping[name] = NodeSlot(self.info.inmapping[name], parent=self)
        return True
    def delete_slot(self, name:str) -> bool:
        if name not in self.info.inmapping:
            return False
        del self.info.inmapping[name]
        del self.Internal_Inmapping[name]
        return True

    @override
    def ToString(self) -> str:
        return f"{self.end_result}"
    @override
    def __repr__(self) -> str:
        return f"{self.GetType()}"

class EndNodeInfo(NodeInfo):
    """
    结束节点信息, 属于结束节点, 用于结束工作流
    """
    @override
    def Instantiate(self) -> Node:
        return EndNode(self)

class WorkflowValidationError(Exception):
    """工作流验证错误"""
    pass

class WorkflowCycleError(WorkflowValidationError):
    """工作流循环错误"""
    pass

class WorkflowTimeoutError(Exception):
    """工作流超时错误"""
    pass

class WorkflowState(BaseModel):
    """工作流状态"""
    node_states:    Dict[int, Dict[str, Any]]   = Field(default_factory=dict)
    current_node:   Optional[int]               = None
    start_time:     float                       = Field(default_factory=time.time)
    end_time:       Optional[float]             = None
    error:          Optional[str]               = None
    is_completed:   bool                        = False

    def save(self, file: tool_file_or_str) -> tool_file:
        """保存工作流状态"""
        file = Wrapper2File(file)
        file.data = self.model_dump()
        file.save_as_json()
        return file

    @classmethod
    def load(cls, file: tool_file_or_str) -> 'WorkflowState':
        """加载工作流状态"""
        file = Wrapper2File(file)
        if not file.exists():
            return cls()
        return cls(**file.load_as_json())

class WorkflowValidator:
    """工作流验证器"""
    @staticmethod
    def validate_workflow(workflow: Workflow) -> None:
        """验证工作流"""
        # 检查循环
        WorkflowValidator._check_cycles(workflow)

        # 检查开始节点和结束节点
        global _Internal_All_EndNodes
        if len(_Internal_All_EndNodes) == 0:
            return
        end_nodes = _Internal_All_EndNodes

        # 检查连接到结束节点的路径上的节点输入是否完整
        WorkflowValidator._check_input_completeness(workflow, end_nodes)

    @staticmethod
    def _check_cycles(workflow: Workflow) -> None:
        """检查工作流中的循环"""
        visited = set()
        recursion_stack = set()

        def dfs(node_id: int) -> None:
            if node_id == -1:
                raise WorkflowCycleError(f"异常节点: {node_id}")
            if node_id in recursion_stack:
                raise WorkflowCycleError(f"检测到循环依赖: {node_id}")
            if node_id in visited:
                return

            visited.add(node_id)
            recursion_stack.add(node_id)

            node = next((n for n in workflow.Nodes if _Internal_GetNodeID(n) == node_id), None)
            if node:
                # 只检查已连接的输出插槽
                for slot in node.Internal_Outmapping.values():
                    if slot.info.targetNode != None:  # 只检查已连接的插槽
                        dfs(_Internal_GetNodeID(slot.info.targetNode))

            recursion_stack.remove(node_id)

        # 从所有节点开始DFS
        for node in workflow.Nodes:
            if node.info.nodeID not in visited:
                dfs(node.info.nodeID)

    @staticmethod
    def _check_input_completeness(workflow: Workflow, end_nodes: List[Node]) -> None:
        """检查连接到结束节点的路径上的节点输入是否完整"""
        visited = set()

        def check_node_inputs(node: Node) -> None:
            if node in visited:
                return
            visited.add(node)

            # 检查所有输入插槽是否都已连接
            for slot_name, slot_info in node.info.inmapping.items():
                if slot_info.targetSlot is None:  # 未连接的输入插槽
                    raise WorkflowValidationError(f"存在上游节点连接不完整: node={node.info.title}, slot={slot_name}")

            # 递归检查所有输入节点
            for slot_info in node.info.inmapping.values():
                if slot_info.targetSlot is not None:  # 只检查已连接的输入节点
                    source_node = next((n for n in workflow.Nodes if n == slot_info.targetNode), None)
                    if source_node:
                        check_node_inputs(source_node)

        # 从所有结束节点开始检查
        for end_node in end_nodes:
            check_node_inputs(end_node)

# 以下为实例化部分

class ResourceNode(StartNode):
    """
    资源节点, 属于开始节点, 用于加载资源
    """
    def __init__(self, info:'ResourceNodeInfo') -> None:
        super().__init__(info)

    @override
    async def _DoRunStep(self) -> Dict[str, context_value_type]|context_value_type:
        resourcesInfo:ResourceNodeInfo = self.info
        resource:str = resourcesInfo.resource
        stats = self.is_start and self.GetResult() is None
        if stats:
            file = tool_file(resource)
            if file.exists():
                return file.load()
        if stats:
            url = tool_url(resource)
            if url.is_valid():
                if url.is_downloadable():
                    return await url.download_async()
        if stats:
            raise ValueError(f"无法获取的资源<{resource}>")
        else:
            return None

class ResourceNodeInfo(StartNodeInfo):
    """
    资源节点信息, 属于开始节点, 用于加载资源
    """
    resource:   str = Field(description="文件地址或url地址", default="unknown")
    @override
    def Instantiate(self) -> Node:
        return ResourceNode(self)

class TextNode(StartNode):
    """
    文本节点, 属于开始节点, 用于加载文本
    """
    def __init__(self, info:'TextNodeInfo') -> None:
        super().__init__(info)

    @override
    async def _DoRunStep(self) -> Dict[str, context_value_type]|context_value_type:
        info:TextNodeInfo = self.info
        return info.text

class TextNodeInfo(StartNodeInfo):
    """
    文本节点信息, 属于开始节点, 用于加载文本
    """
    text:str = Field(description="文本", default="unknown")

    def __init__(
        self,
        text:               Optional[str] = None,
        outmappingName:     Optional[str] = None,
        *,
        targetNodeID:       Optional[int] = None,
        targetSlotName:     Optional[str] = None,
        **kwargs:Any
        ) -> None:
        super().__init__(**kwargs)
        if text is not None:
            self.text = text
        self.inmapping = {}
        if outmappingName is not None or targetNodeID is not None or targetSlotName is not None:
            if outmappingName is None:
                outmappingName = "text"
            if targetNodeID is None:
                targetNodeID = -1
            if targetSlotName is None:
                targetSlotName = "text"
            self.outmapping = {
                outmappingName: NodeSlotInfo(
                    slotName=outmappingName,
                    typeIndicator="str",
                    IsInmappingSlot=False,
                    targetNodeID=targetNodeID,
                    targetSlotName=targetSlotName
                )
            }

    @override
    def Instantiate(self) -> Node:
        return TextNode(self)

class SelectorNode(StartNode):
    """
    选择器节点, 属于开始节点, 用于选择
    """
    def __init__(self, info:'SelectorNodeInfo') -> None:
        super().__init__(info)

    @override
    async def _DoRunStep(self) -> Dict[str, context_value_type]|context_value_type:
        info:SelectorNodeInfo = self.info
        return info.select

class SelectorNodeInfo(StartNodeInfo):
    """
    选择器节点信息, 属于开始节点, 用于选择
    """
    select:str = Field(description="选择器", default="unknown")

    def __init__(
        self,
        select:str = "unknown",
        **kwargs:Any
        ) -> None:
        super().__init__(**kwargs)
        self.select = select


    @override
    def Instantiate(self) -> Node:
        return SelectorNode(self)
