from ..Internal         import *
from pydantic           import BaseModel, Field
from ..Str.Core         import UnWrapper as Unwrapper2Str
from ..File.Core        import (
    tool_file_or_str    as     tool_file_or_str,
    tool_file           as     tool_file,
    Wrapper             as     Wrapper2File,
)
import                         time

__WorkflowManager_instance: Optional['WorkflowManager'] = None
__Internal_GetNodeID:       Callable[['Node'], int]     = lambda node: __WorkflowManager_instance.GetNodeID(node)
__Internal_GetNode:         Callable[[int], 'Node']     = lambda id: __WorkflowManager_instance.GetNode(id)
__Internal_ContainsNode:    Callable[[int], bool]       = lambda id: __WorkflowManager_instance.ContainsNode(id)

type context_type = Dict[str, Any]
'''
上下文中特殊的键值对:
    result: 步骤的输出结果(当步骤执行结果不是一个字典时, 结果将被保存到此键值对上)
    error: 步骤的错误信息(当步骤执行失败时, 错误信息将被保存到此键值对上)
'''
type input_mapping_type = Dict[str, str]
type output_mapping_type = Dict[str, str]
type action_label_type = str

action_name:Literal["action"] = "action" # 动作名称

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
    def CopyFromNode(self, node:Self) -> None:
        pass
    @override
    def ToString(self) -> str:
        return self.title
    @override
    def SymbolName(self) -> str:
        return f"{self.GetType().__name__}<typename={self.typename}, title={self.title}>"
    @virtual
    def Instantiate(self) -> 'Node':
        raise NotImplementedError(f"节点类型<{self.__class__.__name__}>未实现Instantiate方法")

class NodeSlot(left_value_reference[NodeSlotInfo], BaseBehavior):
    """
    节点插槽
    """
    @property
    def info(self) -> NodeSlotInfo:
        return self.ref_value
    __parentNode: Optional['Node'] = None

    __IsDirty:bool = False
    def SetDirty(self) -> None:
        self.__IsDirty = True

    @classmethod
    def link(cls, left:Self, right:Self) -> None:
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

    def link_to(self, other:Optional[Self]) -> None:
        if other is None:
            NodeSlot.Unlink(self)
        else:
            NodeSlot.link(self, other)

    def setup_from_info(self, info:NodeSlotInfo) -> None:
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
               f"parent={self.__parentNode.SymbolName()}>"

class Node(left_value_reference[NodeInfo], BaseBehavior):
    """
    节点
    """
    @property
    def info(self) -> NodeInfo:
        return self.ref_value

    def setup_from_info(self, info:NodeInfo) -> None:
        self.info = info
        info.nodeID = __Internal_GetNodeID(self)
        info.node = self

    def link_inslot_to_other_node_outslot(self, other:Self, slotName:str, targetSlotName:str) -> None:
        NodeSlot.link(self.info.outmapping[slotName], other.info.inmapping[targetSlotName])
    def link_outslot_to_other_node_inslot(self, other:Self, slotName:str, targetSlotName:str) -> None:
        NodeSlot.link(self.info.inmapping[slotName], other.info.outmapping[targetSlotName])
    def unlink_inslot(self, slotName:str) -> None:
        NodeSlot.Unlink(self.info.inmapping[slotName])
    def unlink_outslot(self, slotName:str) -> None:
        NodeSlot.Unlink(self.info.outmapping[slotName])

class Workflow(BaseModel, any_class):
    """
    工作流信息
    """
    Datas:        List[NodeInfo] = Field(description="节点信息", default=[])
    Nodes:        List[Node]     = Field(description="节点, 此变量需要手动同步, nodeID的懒加载目标", default=[], exclude=True)

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

    __callbackDatas:List[Tuple[str, Callable[[dict], None]]] = []

    def CreateNode(self, info:NodeInfo) -> Node:
        node:Node = info.Instantiate()
        self.workflow.Nodes.append(node)
        node.setup_from_info(info)
        return node

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
        return self
    #TODO
    def BuildWorkflow(self) -> Self:
        return self

    def RefreshImmediate(self) -> Self:
        self.ClearWorkflow()
        self.BuildWorkflow()
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

class WorkflowGraphStep(any_class):
    """
    工作流步骤类，表示工作流中的一个步骤。
    """

    def __init__(
        self,
        name:                   str,
        description:            str,
        action:                 action_label_type,
        input_mapping:          input_mapping_type      = None,
        output_mapping:         output_mapping_type     = None,
        retry_count:            int                     = 3,
        retry_delay:            int                     = 5,
    ):
        """
        初始化工作流步骤。

        参数:
            name: 步骤名称
            description: 步骤描述
            action: 步骤执行的动作的标签, 指向函数包装器
            input_mapping: 输入映射，将工作流变量映射到步骤输入, 是函数的参数
            output_mapping: 输出映射，将步骤输出映射到工作流变量, 是函数的输出
            retry_count: 重试次数
            retry_delay: 重试延迟（秒）
        """
        self.name = name
        self.description = description
        self.action = action
        self.input_mapping = input_mapping or {}
        self.output_mapping = output_mapping or {}
        self.retry_count = retry_count
        self.retry_delay = retry_delay

    def execute(self, context: context_type) -> context_type:
        """
        执行步骤。

        参数:
            context: 工作流上下文
        返回:
            更新后的上下文
        """
        # 准备输入
        inputs = {}
        for step_input, context_var in self.input_mapping.items():
            if context_var in context:
                inputs[step_input] = context[context_var]

        # 执行动作
        for attempt in range(self.retry_count):
            try:
                if WorkflowActionWrapper.ContainsActionWrapper(self.action):
                    result = WorkflowActionWrapper.GetActionWrapper(self.action)(**inputs)
                elif self.action in context and callable(context[self.action]):
                    # 命令标签未找到，但上下文中存在该标签名称的函数可以实现命令执行逻辑
                    result = context[self.action](**inputs)
                else:
                    # 命令标签未找到，且上下文中不存在该标签名称的函数可以实现命令执行逻辑
                    # 输出命令标签, 期望有函数可以处理该结果
                    result = {"output": f"{action_name}: {self.action}"}

                # 更新上下文
                # 遍历输出映射中的每个键值对
                for output_var, context_var in self.output_mapping.items():
                    # 如果结果是字典类型且输出变量存在于结果中
                    # 将结果字典中对应的值映射到上下文变量
                    if isinstance(result, dict) and output_var in result:
                        context[context_var] = result[output_var]
                    # 如果输出变量是"result"且结果不是字典类型
                    # 直接将结果值映射到特定的上下文变量
                    elif output_var == "result" and not isinstance(result, dict):
                        context[context_var] = result
                    # 找不到合适的情况
                    else:
                        pass
                    #else:
                    #    raise Exception(f"步骤 {self.name} 执行失败，输出类型不正确")

                return context
            except Exception as e:
                print(f"步骤 {self.name} 执行失败 (尝试 {attempt+1}/{self.retry_count}): {str(e)}")
                if attempt < self.retry_count - 1:
                    time.sleep(self.retry_delay)

        # 所有重试都失败
        raise Exception(f"步骤 {self.name} 执行失败，已重试 {self.retry_count} 次")

    def Serialize(self) -> Dict[str, Any]:
        return {
            "name": self.name,
            "description": self.description,
            "action": self.action,
            "input_mapping": self.input_mapping,
            "output_mapping": self.output_mapping,
            "retry_count": self.retry_count,
            "retry_delay": self.retry_delay,
        }

    def SerializeToFile(self, file:tool_file_or_str, save_immediate:bool = True) -> bool:
        file = Wrapper2File(file)
        if not Wrapper2File(file.dirpath).exists():
            return False
        file.data = self.Serialize()
        if save_immediate:
            file.save_as_json()
        return True

    def DeserializeFromFile(self, file:tool_file_or_str) -> bool:
        file = Wrapper2File(file)
        if not file.exists():
            return False
        data:Dict[str, Any] = file.load_as_json()
        self.name = data["name"]
        self.description = data["description"]
        self.action = data["action"]
        self.input_mapping = data["input_mapping"]
        self.output_mapping = data["output_mapping"]
        self.retry_count = data["retry_count"]
        self.retry_delay = data["retry_delay"]
        return True

    @classmethod
    def Deserialize(cls, data: Dict[str, Any]) -> Self:
        return cls(
            name=data["name"],
            description=data["description"],
            action=data["action"],
            input_mapping=data["input_mapping"],
            output_mapping=data["output_mapping"],
            retry_count=data["retry_count"],
            retry_delay=data["retry_delay"],
        )

class WorkflowGraph(left_value_reference[List[WorkflowGraphStep]]):
    """
    工作流类，表示一系列步骤的执行流程。
    """

    def __init__(
        self,
        name:           str,
        description:    str,
        steps:          List[WorkflowGraphStep] = None,
    ):
        """
        初始化工作流。

        参数:
            name: 工作流名称
            description: 工作流描述
            steps: 工作流步骤列表
        """
        self.name = name
        self.description = description
        super().__init__(steps or [])
        self.context = {}

    @property
    def steps(self) -> List[WorkflowGraphStep]:
        return self.ref_value

    def add_step(self, step: WorkflowGraphStep) -> Self:
        """
        添加步骤到工作流。

        参数:
            step: 工作流步骤
        返回:
            工作流对象（链式调用）
        """
        self.steps.append(step)
        return self

    def execute(self, initial_context: context_type = None) -> context_type:
        """
        执行工作流。

        参数:
            initial_context: 初始上下文
        返回:
            最终上下文
        """
        self.context = initial_context or {}

        print(f"开始执行工作流: {self.name}")
        print(f"描述: {self.description}")
        print(f"步骤数量: {len(self.steps)}")

        for i, step in enumerate(self.steps):
            print(f"执行步骤 {i+1}/{len(self.steps)}: {step.name}")
            print(f"描述: {step.description}")

            try:
                self.context = step.execute(self.context)
                print(f"步骤 {step.name} 执行成功")
            except Exception as e:
                print(f"步骤 {step.name} 执行失败: {str(e)}")
                # 可以在这里添加错误处理逻辑
                raise

        print(f"工作流 {self.name} 执行完成")
        return self.context

    @classmethod
    def class_name(cls) -> str:
        """返回类名"""
        return "Workflow"

    def Serialize(self) -> Dict[str, Any]:
        """序列化工作流"""
        return {
            "name": self.name,
            "description": self.description,
            "steps": [step.Serialize() for step in self.steps],
        }

    def SerializeToFile(self, file:tool_file_or_str, save_immediate:bool = True) -> bool:
        """序列化工作流到文件"""
        file = Wrapper2File(file)
        if not Wrapper2File(file.dirpath).exists():
            return False
        file.data = self.Serialize()
        if save_immediate:
            file.save_as_json()

    def DeserializeFromFile(self, file:tool_file_or_str) -> bool:
        """从文件反序列化工作流"""
        file = Wrapper2File(file)
        if not file.exists():
            return False
        data:Dict[str, Any] = file.load_as_json()
        self.name = data["name"]
        self.description = data["description"]
        self.steps = [WorkflowGraphStep.Deserialize(step) for step in data["steps"]]
        return True

    @classmethod
    def Deserialize(cls, data: Dict[str, Any]) -> Self:
        """从字典反序列化工作流"""
        workflow = cls(data["name"], data["description"])
        workflow.steps = [WorkflowGraphStep.Deserialize(step) for step in data["steps"]]
        return workflow
