from ..Internal         import *
from ..Lang.Core        import run_until_complete
from pydantic           import BaseModel, Field
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

__WorkflowManager_instance: Optional['WorkflowManager'] = None
__Internal_GetNodeID:       Callable[['Node'], int]     = lambda node: __WorkflowManager_instance.GetNodeID(node)
__Internal_GetNode:         Callable[[int], 'Node']     = lambda id: __WorkflowManager_instance.GetNode(id)
__Internal_ContainsNode:    Callable[[int], bool]       = lambda id: __WorkflowManager_instance.ContainsNode(id)

type context_type = Dict[str, Any]
type context_key_type = str
type context_value_type = Any
type context_keyword = Literal["result", "error"]
'''
上下文中特殊的键值对:
    result: 步骤的输出结果(当步骤执行结果不是一个字典时, 结果将被保存到此键值对上)
    error: 步骤的错误信息(当步骤执行失败时, 错误信息将被保存到此键值对上)
'''
type input_mapping_type = Dict[str, str]
type output_mapping_type = Dict[str, str]
type action_label_type = str

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
    parentNode:     'Node'                  = Field(description="所属的父节点", default=None, exclude=True)
    slot:           'NodeSlot'              = Field(description="所属的插槽", default=None, exclude=True)
    slotName:       str                     = Field(description="插槽名称", default="unknown")
    targetNode:     'Node'                  = Field(description="目标节点, 此变量需要手动同步, targetNodeID的懒加载目标", default=None, exclude=True)
    targetSlot:     'NodeSlot'              = Field(description="目标插槽, 此变量需要手动同步, targetSlotName的懒加载目标", default=None, exclude=True)
    targetNodeID:   int                     = Field(description="目标节点ID", default=-1)
    targetSlotName: str                     = Field(description="目标插槽名称", default="unknown")
    typeIndicator:  str                     = Field(description="类型指示器, 此插槽的类型", default="unknown")
    IsInmappingSlot:bool                    = Field(description="是否为输入映射插槽", default=False)

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
        self.nodeId = __Internal_GetNodeID(node)
    @override
    def ToString(self) -> str:
        return self.title
    @override
    def SymbolName(self) -> str:
        return f"{self.GetType().__name__}<typename={self.typename}, title={self.title}>"
    @virtual
    def Instantiate(self) -> 'Node':
        raise NotImplementedError(f"节点类型<{self.__class__.__name__}>未实现Instantiate方法")

class WorkflowEvent(any_class):
    @override
    def SymbolName(self) -> str:
        return self.GetType().__name__

class WorkflowStartEvent(WorkflowEvent):
    @override
    def SymbolName(self) -> str:
        return "StartEvent"

class WorkflowStopEvent(WorkflowEvent):
    @override
    def SymbolName(self) -> str:
        return "StopEvent"

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

        left.info.targetNodeID = __Internal_GetNodeID(right.info.targetNode)
        right.info.targetNodeID = __Internal_GetNodeID(left.info.targetNode)

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
    
class Node(left_value_reference[NodeInfo], BaseBehavior):
    """
    节点
    """
    @property
    def info(self) -> NodeInfo:
        return self.ref_value

    __m_Inmapping:Dict[str, NodeSlot] = {}
    __m_Outmapping:Dict[str, NodeSlot] = {}
    
    @sealed
    def ClearLink(self) -> None:
        self.__m_Inmapping.clear()
        self.__m_Outmapping.clear()
    @sealed
    def BuildLink(self) -> None:
        for slot_name, info in self.info.inmapping.items():
            
            self.__m_Inmapping[slot_name] = NodeSlot(info.TemplateClone())
        for slot_name, info in self.info.outmapping.items():
            self.__m_Outmapping[slot_name] = NodeSlot(info.TemplateClone())
    @sealed
    def RefreshImmediate(self) -> None:
        # TODO 在断连之前需要保存连接信息, 在重建时恢复连接
        self.ClearLink()
        self.BuildLink()

    def SetupFromInfo(self, info:NodeInfo) -> None:
        self.info = info
        info.nodeID = __Internal_GetNodeID(self)
        info.node = self

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
    def OnStartEvent(self, event:Any) -> None:
        if isinstance(event, WorkflowStartEvent):
            self.__is_start = True
    @virtual
    def OnStopEvent(self, event:Any) -> None:
        if isinstance(event, WorkflowStopEvent):
            self.__is_start = False
            self.ClearResult()
            self.ClearParameters()
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
        for slot_name, info in self.info.inmapping.items():
            slot = info.slot
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
        
class Workflow(BaseModel, any_class):
    """
    工作流信息
    """
    Datas:        List[NodeInfo] = Field(description="节点信息", default=[])
    Nodes:        List[Node]     = Field(description="节点, 此变量需要手动同步, nodeID的懒加载目标", default=[], exclude=True)
    
    @classmethod
    def CreateTemplate(cls) -> Self:
        return cls(
            Datas=[
                EndNodeInfo()
            ]
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

class WorkflowManager(left_value_reference[Workflow]):
    """
    工作流管理器
    """
    def __init__(self, workflow:Workflow):
        super().__init__(workflow)
        global __WorkflowManager_instance
        if __WorkflowManager_instance is not None:
            raise Exception("WorkflowManager 只能有一个实例")
        __WorkflowManager_instance = self
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
        global __WorkflowManager_instance
        if __WorkflowManager_instance is None:
            __WorkflowManager_instance = WorkflowManager(None)
        return __WorkflowManager_instance

    def ClearWorkflow(self) -> Self:
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

    def GetNode(self, id:int) -> NodeInfo:
        if id < 0:
            return None
        return next((node for node in self.ref_value.Datas if node.nodeID == id), None)

    def GetNodeID(self, node:Optional[Node]) -> int:
        if node is None:
            return -1
        return self.workflow.Datas.index(node.info)

    def SaveWorkflow(self, file:tool_file_or_str) -> tool_file:
        file = Wrapper2File(file)
        if not Wrapper2File(file.dirpath).exists():
            raise Exception(f"{file.dirpath} 不存在")
        currentWorkflow:Workflow = self.workflow
        currentWorkflow.Datas.clear()
        for node in currentWorkflow.Nodes:
            node.info.CopyFromNode(node)
            currentWorkflow.Datas.append(node.info)
        file.data = currentWorkflow
        file.save_as_json()
        return file

    def LoadWorkflow(self, file:tool_file_or_str) -> Workflow:
        file = Wrapper2File(file)
        if not file.exists():
            raise Exception(f"{file} 不存在")
        workflow:Workflow = file.load_as_json()
        self.ClearWorkflow()
        self.ref_value = Workflow()
        for info in workflow.Datas:
            workflow.Nodes.append(self.CreateNode(info))
        return workflow

    @property
    def state(self) -> 'WorkflowState':
        return self.__state

    def set_timeout(self, seconds: float) -> None:
        """设置工作流超时时间"""
        self.__timeout = seconds

    async def RunWorkflow(self) -> None:
        """运行工作流"""
        try:
            self.__state = WorkflowState()
            self.__state.start_time = time.time()
            
            # 启动工作流
            await EndNode.Start()
            
            # 等待完成或超时
            while not self.__state.is_completed:
                if time.time() - self.__state.start_time > self.__timeout:
                    raise WorkflowTimeoutError(f"工作流执行超时: {self.__timeout}秒")
                await asyncio.sleep(0.1)
                
        except Exception as e:
            self.__state.error = str(e)
            self.__state.end_time = time.time()
            raise
        finally:
            self.__state.end_time = time.time()
            self.__state.is_completed = True

    def SaveState(self, file: tool_file_or_str) -> tool_file:
        """保存工作流状态"""
        return self.__state.save(file)

    def LoadState(self, file: tool_file_or_str) -> 'WorkflowState':
        """加载工作流状态"""
        self.__state = WorkflowState.load(file)
        return self.__state

class StartNode(Node):
    """
    开始节点, 工作流的起点
    """
    @override
    async def _DoRunStep(self) -> Dict[str, context_value_type]|context_value_type:
        raise NotImplementedError(f"开始节点<{self.__class__.__name__}>未实现_DoRunStep方法")
    
class StartNodeInfo(NodeInfo):
    """
    开始节点信息, 属于开始节点, 用于开始工作流
    """
    def __init__(self, **kwargs:Any) -> None:
        super().__init__(**kwargs)
    @override
    def Instantiate(self) -> Node:
        raise NotImplementedError(f"开始节点<{self.__class__.__name__}>未实现Instantiate方法")

class StepNode(Node):
    """
    步骤节点, 在工作流中触发
    """
    @override
    async def _DoRunStep(self) -> Dict[str, context_value_type]|context_value_type:
        func = WorkflowActionWrapper.GetActionWrapper(self.info.funcname)
        return func(**self.GetParameters())
    
class StepNodeInfo(NodeInfo):
    """
    步骤节点, 在工作流中触发
    """
    funcname:action_label_type = Field(description="函数键名", default="unknown")
    
    def __init__(
        self, 
        funcname:action_label_type,
        ) -> None:
        '''
        funcname: action_label_type
            由WorkflowActionWrapper包装的函数所使用的key, 通过WorkflowActionWrapper.GetActionWrapper(funcname)获取包装的函数
        '''
        super().__init__()
        self.funcname = funcname
    @override
    def Instantiate(self) -> Node:
        return StepNode(self)

__Internal_All_EndNodes:List['EndNode'] = []
__Internal_Task_Count:int = 0

class EndNode(Node):
    """
    结束节点, 工作流的终端
    """
    def __init__(self, **kwargs:Any) -> None:
        super().__init__(**kwargs)
        __Internal_All_EndNodes.append(self)
    def __del__(self):
        __Internal_All_EndNodes.remove(self)
    @override
    async def _DoRunStep(self) -> Dict[str, context_value_type]|context_value_type:
        return await self.GetParameters()
    
    @override
    def OnStartEvent(self, event:Any) -> None:
        super().OnStartEvent(event)
        if isinstance(event, WorkflowStartEvent):
            run_until_complete(self.GetParameters())
            global __Internal_Task_Count
            __Internal_Task_Count -= 1
    @override
    def OnStopEvent(self, event:Any) -> None:
        super().OnStopEvent(event)
    
    @classmethod
    async def Start(cls) -> None:
        '''
        开始工作流
        '''
        global __Internal_All_EndNodes
        if len(__Internal_All_EndNodes) == 0:
            return
        global __Internal_Task_Count    
        __Internal_Task_Count = len(__Internal_All_EndNodes)
        __Internal_All_EndNodes[0].Broadcast(WorkflowStartEvent())
        while __Internal_Task_Count > 0:
            await asyncio.sleep(0.1)
        __Internal_All_EndNodes[0].Broadcast(WorkflowStopEvent())
    @classmethod
    async def Stop(cls) -> None:
        '''
        打断任务, 直接停止工作流
        '''
        global __Internal_Task_Count
        __Internal_Task_Count = 0
    
class EndNodeInfo(NodeInfo):
    """
    结束节点信息, 属于结束节点, 用于结束工作流
    """
    def __init__(self, **kwargs:Any) -> None:
        super().__init__(**kwargs)
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
        has_end_node = False
        
        for node in workflow.Nodes:
            if isinstance(node, EndNode):
                has_end_node = True
                
        if not has_end_node:
            raise WorkflowValidationError("工作流缺少结束节点(EndNode)")

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

# 以下为实例化部分

class ResourceNode(StartNode):
    """
    资源节点, 属于开始节点, 用于加载资源
    """
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
    def __init__(
        self, 
        resource:str,
        **kwargs
        ) -> None:
        super().__init__(**kwargs)
        self.resource = resource
    @override
    def Instantiate(self) -> Node:
        return ResourceNode(self)

class TextNode(StartNode):
    """
    文本节点, 属于开始节点, 用于加载文本
    """
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
        text:str,
        **kwargs
        ) -> None:
        super().__init__(**kwargs)
        self.text = text
    @override
    def Instantiate(self) -> Node:
        return TextNode(self)
