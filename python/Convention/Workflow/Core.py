from ..Internal         import *
from pydantic           import BaseModel, Field
from ..Str.Core         import UnWrapper as Unwrapper2Str
from ..File.Core        import (
    tool_file_or_str    as tool_file_or_str,
    tool_file           as tool_file,
    Wrapper             as Wrapper2File,
)
import                     time


class GraphNodeSlotInfo(BaseModel, any_class):
    """
    插槽信息
    """
    slotName:       str = Field(description="插槽名称", default="unknown")
    targetNodeID:   int = Field(description="目标节点ID", default=-1)
    targetSlotName: str = Field(description="目标插槽名称", default="unknown")
    typeIndicator:  str = Field(description="类型指示器, 此插槽的类型", default="unknown")
    IsInmappingSlot:bool = Field(description="是否为输入映射插槽", default=False)

    def TemplateClone(self) -> Self:
        return self.model_copy(deep=True)

class GraphNodeInfo(BaseModel, any_class):
    """
    节点信息
    """
    nodeID:         int                             = Field(description="节点ID", default=-1)
    typename:       str                             = Field(description="节点类型", default="unknown")
    title:          str                             = Field(description="节点标题", default="unknown")
    inmapping:      Dict[str, GraphNodeSlotInfo]    = Field(description="输入映射", default={})
    outmapping:     Dict[str, GraphNodeSlotInfo]    = Field(description="输出映射", default={})

    def TemplateClone(self) -> Self:
        return self.model_copy(deep=True)

class Workflow(BaseModel, any_class):
    """
    工作流信息
    """
    nodes:        List[GraphNodeInfo] = Field(description="节点信息", default=[])


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

__WorkflowManager_instance:Optional['WorkflowManager'] = None
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

    @classmethod
    def GetInstance(cls) -> Self:
        global __WorkflowManager_instance
        if __WorkflowManager_instance is None:
            __WorkflowManager_instance = WorkflowManager(None)
        return __WorkflowManager_instance


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
