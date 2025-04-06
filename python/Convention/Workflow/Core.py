from ..Internal         import *
from ..Lang.Core        import run_until_complete
from pydantic           import BaseModel, Field, GetCoreSchemaHandler
from pydantic_core      import core_schema
from ..Lang.EasySave    import EasySave
from ..Str.Core         import UnWrapper as Unwrapper2Str
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

# 将全局变量声明移到文件顶部
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

__all__workflow_action_wrappers__:Dict[action_label_type, 'WorkflowActionWrapper'] = {}

class NodeSlotInfo(BaseModel, any_class):
    """
    插槽信息
    """
    parentNode:         'Node'     = Field(description="所属的父节点", default=None, exclude=True)
    slot:               'NodeSlot' = Field(description="所属的插槽", default=None, exclude=True)
    slotName:           str        = Field(description="插槽名称", default="unknown")
    targetNode:         'Node'     = Field(description="目标节点, 此变量需要手动同步, targetNodeID的懒加载目标",
                                           default=None, exclude=True)
    targetSlot:         'NodeSlot' = Field(description="目标插槽, 此变量需要手动同步, targetSlotName的懒加载目标",
                                           default=None, exclude=True)
    targetNodeID:       int        = Field(description="目标节点ID", default=-1)
    targetSlotName:     str        = Field(description="目标插槽名称", default="unknown")
    typeIndicator:      str        = Field(description="类型指示器, 此插槽的类型", default="unknown")
    IsInmappingSlot:    bool       = Field(description="是否为输入映射插槽", default=False)

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

class NodeInfo(BaseModel, any_class):
    """
    节点信息
    """
    node:           'Node'                  = Field(description="节点, 此变量需要手动同步, nodeID的懒加载目标", default=None, exclude=True)
    nodeID:         int                     = Field(description="节点ID", default=-1)
    typename:       str                     = Field(description="节点类型", default="unknown")
    title:          str                     = Field(description="节点标题", default="unknown")
    inmapping:      Dict[str, NodeSlotInfo] = Field(description="输入映射", default={})
    outmapping:     Dict[str, NodeSlotInfo] = Field(description="输出映射", default={})
    position:       Tuple[float, float]     = Field(description="节点位置", default=(0, 0))

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
        result:NodeInfo = NodeInfo(nodeID=self.nodeID, typename=self.typename, title=self.title, position=self.position)
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
        raise NotImplementedError(f"节点类型<{self.GetType().__name__}, typename={self.typename}, trackback={self.generate_trackback}>未实现Instantiate方法")

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

    def __init__(self, error:Exception, from_:any_class):
        self.error = error
        self.from_ = from_

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

    @classmethod
    def Link(cls, left:Self, right:Self) -> None:
        if left.info.IsInmappingSlot==right.info.IsInmappingSlot:
            raise ValueError(f"相同映射的插槽<{left.info.slotName}>和<{right.info.slotName}>不能连接")
        if left.info.typeIndicator!=right.info.typeIndicator:
            raise ValueError(f"类型不匹配的插槽<{left.info.slotName}>和<{right.info.slotName}>不能连接")
        if left.info.targetNodeID==right.info.targetNodeID:
            raise ValueError(f"目标节点ID相同的插槽<{left.info.slotName}>和<{right.info.slotName}>不能连接")

        left.info.targetSlot = right
        right.info.targetSlot = left

        left.info.targetSlotName = right.info.slotName
        right.info.targetSlotName = left.info.slotName

        left.info.targetNode = right.info.parentNode
        right.info.targetNode = right.info.parentNode

        left.info.targetNodeID = _Internal_GetNodeID(right.info.targetNode)
        right.info.targetNodeID = _Internal_GetNodeID(left.info.targetNode)

        left.SetDirty()
        right.SetDirty()
    @classmethod
    def Unlink(cls, slot:Self) -> None:
        targetSlot:Optional[NodeSlot] = slot.info.targetSlot
        slot.info.targetSlot = None
        slot.info.targetNode = None
        slot.info.targetNodeID = -1
        if targetSlot is not None:
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

    def __init__(self, info:NodeSlotInfo):
        super().__init__(info)

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
            return await self.info.targetSlot.GetParameter()
        else:
            await self.info.parentNode.RunStep()
            while self.__parameter is None and self.__is_start:
                await asyncio.sleep(0.1)
            if self.__is_start:
                return self.__parameter.ref_value
            else:
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
    def __init__(self, info:NodeInfo) -> None:
        super().__init__(info)

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

    Internal_Inmapping:Dict[str, NodeSlot] = {}
    Internal_Outmapping:Dict[str, NodeSlot] = {}

    @sealed
    def ClearLink(self) -> None:
        self.Internal_Inmapping.clear()
        self.Internal_Outmapping.clear()
    @sealed
    def BuildLink(self) -> None:
        for slot_name, info in self.info.inmapping.items():
            self.Internal_Inmapping[slot_name] = NodeSlot(info.TemplateClone())
        for slot_name, info in self.info.outmapping.items():
            self.Internal_Outmapping[slot_name] = NodeSlot(info.TemplateClone())
    @sealed
    def RefreshImmediate(self) -> None:
        # TODO 在断连之前需要保存连接信息, 在重建时恢复连接
        self.ClearLink()
        self.BuildLink()

    def SetupFromInfo(self, info:NodeInfo) -> None:
        self.ClearLink()
        self.ref_value = info
        self.info.nodeID = _Internal_GetNodeID(self)
        self.info.node = self
        self.BuildLink()

    def link_inslot_to_other_node_outslot(self, other:Self, slotName:str, targetSlotName:str) -> None:
        NodeSlot.Link(self.info.outmapping[slotName], other.info.inmapping[targetSlotName])
    def link_outslot_to_other_node_inslot(self, other:Self, slotName:str, targetSlotName:str) -> None:
        NodeSlot.Link(self.info.inmapping[slotName], other.info.outmapping[targetSlotName])
    def unlink_inslot(self, slotName:str) -> None:
        NodeSlot.Unlink(self.info.inmapping[slotName])
    def unlink_outslot(self, slotName:str) -> None:
        NodeSlot.Unlink(self.info.outmapping[slotName])

    __is_start:bool = False
    @virtual
    def OnStartEvent(self, event:any_class) -> None:
        if isinstance(event, WorkflowStartEvent):
            self.__is_start = True
            if event.verbose:
                print_colorful(ConsoleFrontColor.GREEN, f"{self.__class__.__name__}<id={_Internal_GetNodeID(self)}> catch event: {event.GetType()}")
    @virtual
    def OnStopEvent(self, event:any_class) -> None:
        if isinstance(event, WorkflowStopEvent):
            self.__is_start = False
            self.ClearResult()
            self.ClearParameters()
            if event.verbose:
                print_colorful(ConsoleFrontColor.GREEN, f"{self.__class__.__name__}<id={_Internal_GetNodeID(self)}> catch event: {event.GetType()}")
    @property
    def is_start(self) -> bool:
        return self.__is_start

    __parameters:Optional[Dict[str, context_value_type]] = None
    __results:Optional[Dict[str, context_value_type]]|context_value_type = None
    @sealed
    def GetResult(self) -> context_value_type|None:
        return self.__results
    @sealed
    async def BuildParameter(self) -> None:
        '''
        异步函数, 从inslots中构建参数
        '''
        parameters = {}
        for slot_name, slot in self.Internal_Inmapping.items():
            value = await slot.GetParameter()
            parameters[slot_name] = value
        self.__parameters = parameters
    @sealed
    async def GetParameters(self) -> Dict[str, context_value_type]:
        if not self.__is_start:
            return {}
        if self.__parameters is None:
            await self.BuildParameter()
        return self.__parameters
    @sealed
    def ClearParameters(self) -> None:
        self.__parameters = None
    @sealed
    def ClearResult(self) -> None:
        self.__results = None
    @sealed
    async def RunStep(self) -> None:
        if self.__results is not None:
            return
        try:
            parameters = await self.GetParameters()
            self.__results = await self._DoRunStep(parameters)
            if isinstance(self.__results, dict):
                for key, value in self.__results.items():
                    self.info.outmapping[key].slot.SetParameter(value)
            else:
                if len(self.info.outmapping.items()) == 1:
                    self.info.outmapping[next(iter(self.info.outmapping.keys()))].slot.SetParameter(self.__results)
                else:
                    self.info.outmapping["result"].slot.SetParameter(self.__results)
        except Exception as e:
            self.Broadcast(WorkflowErrorEvent(e, self))

    @virtual
    async def _DoRunStep(self) -> Dict[str, context_value_type]|context_value_type:
        raise NotImplementedError(f"节点类型<{self.__class__.__name__}>未实现_run_step方法")

    @override
    def __repr__(self) -> str:
        return f"Node<{self.info.title}, id={_Internal_GetNodeID(self)}>"

class Workflow(BaseModel, any_class):
    """
    工作流信息
    """
    Datas:        List[NodeInfo] = Field(description="节点信息", default=[])
    Nodes:        List[Node]     = Field(description="节点, 此变量需要手动同步, nodeID的懒加载目标",
                                         default=[], exclude=True)

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
    def Create(cls, *args:NodeInfo|List[NodeInfo]) -> Self:
        return cls(
            Datas=to_list(args)
        )

class WorkflowActionWrapper(left_value_reference[Callable], invoke_callable):
    def __init__(self, name:action_label_type, action:Callable):
        super().__init__(action)
        self.name = name
        __all__workflow_action_wrappers__[name] = self
    def __del__(self):
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

_Internal_All_EndNodes:List['EndNode'] = []
_Internal_Task_Count:int = 0

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
        node:Node = info.Instantiate()
        self.workflow.Nodes.append(node)
        node.SetupFromInfo(info)
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

    def GetNode[NodeT:Node](self, id:int) -> NodeT:
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
        # 以复制的形式重建
        self.ref_value = Workflow()
        for info in workflow.Datas:
            self.CreateNode(info)
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
            return
        global _Internal_Task_Count
        _Internal_Task_Count = len(_Internal_All_EndNodes)
        self.Broadcast(WorkflowStartEvent(verbose=verbose), "OnStartEvent")
        while _Internal_Task_Count > 0:
            await asyncio.sleep(0.1)
        self.Broadcast(WorkflowStopEvent(verbose=verbose), "OnStopEvent")

    async def RunWorkflow(self, verbose:bool=False) -> None:
        """运行工作流"""
        print_colorful(ConsoleFrontColor.BLUE, "运行工作流")
        try:
            self.__state = WorkflowState()
            self.__state.start_time = time.time()

            # 启动工作流
            await self._DoStart(verbose)

            # 等待完成或超时
            while not self.__state.is_completed:
                if time.time() - self.__state.start_time > self.__timeout:
                    self.StopWorkflow()
                    raise WorkflowTimeoutError(f"工作流执行超时: {self.__timeout}秒")
                await asyncio.sleep(0.1)

        except Exception as e:
            self.__state.error = str(e)
            self.__state.end_time = time.time()
            raise
        finally:
            self.__state.end_time = time.time()
            self.__state.is_completed = True
        print_colorful(ConsoleFrontColor.BLUE, f"工作流完成, 用时: {time.time() - self.__state.start_time}秒, 异常状态: {self.__state.error}")

    def StopWorkflow(self, file: Optional[tool_file_or_str]=None) -> None:
        '''
        打断任务, 直接停止工作流
        '''
        # if file is not None:
        #     self.SaveState(file)
        global _Internal_Task_Count
        _Internal_Task_Count = 0

    @override
    def __repr__(self) -> str:
        return f"{self.__class__.__name__}"

    # def SaveState(self, file: tool_file_or_str) -> tool_file:
    #     """保存工作流状态"""
    #     return self.__state.save(file)

    # def LoadState(self, file: tool_file_or_str) -> 'WorkflowState':
    #     """加载工作流状态"""
    #     self.__state = WorkflowState.load(file)
    #     return self.__state

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
            result = await func(**await self.GetParameters())
            if self._verbose:
                print(f"{self.__class__.__name__}<id={_Internal_GetNodeID(self)}> end func: {self.info.funcname}, result: {result}")
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
        self.end_result = await self.GetParameters()
        if len(self.end_result) == 1:
            self.end_result = self.end_result[0]
        elif len(self.end_result) == 0:
            self.end_result = None
        return self.end_result

    @override
    def OnStartEvent(self, event:Any) -> None:
        super().OnStartEvent(event)
        if isinstance(event, WorkflowStartEvent):
            async def _get_params():
                await self.GetParameters()
                global _Internal_Task_Count
                _Internal_Task_Count -= 1
            asyncio.create_task(_get_params())
    @override
    def OnStopEvent(self, event:Any) -> None:
        super().OnStopEvent(event)

    def add_slot(self, name:str, typeIndicator:str|type) -> bool:
        if name in self.info.inmapping:
            return False
        self.info.inmapping[name] = NodeSlotInfo(
            parentNode=self,
            slotName=name,
            typeIndicator=f"{typeIndicator}",
            IsInmappingSlot=True
        )
        return True
    def delete_slot(self, name:str) -> bool:
        if name not in self.info.inmapping:
            return False
        del self.info.inmapping[name]
        return True

    @override
    def ToString(self) -> str:
        return f"{self.end_result}"
    @override
    def __repr__(self) -> str:
        return f"{self.__class__.__name__}<result={self.end_result}, id={_Internal_GetNodeID(self)}>"

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
            if node_id in recursion_stack:
                raise WorkflowCycleError(f"检测到循环依赖: {node_id}")
            if node_id in visited:
                return

            visited.add(node_id)
            recursion_stack.add(node_id)

            node = next((n for n in workflow.Nodes if n.info.nodeID == node_id), None)
            if node:
                # 只检查已连接的输出插槽
                for slot_info in node.info.outmapping.values():
                    if slot_info.targetNodeID != -1:  # 只检查已连接的插槽
                        dfs(slot_info.targetNodeID)

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
    @override
    def Instantiate(self) -> Node:
        return TextNode(self)
