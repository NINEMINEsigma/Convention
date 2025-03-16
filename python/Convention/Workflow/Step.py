from .Interface     import *
from ..File.Core    import (
    tool_file_or_str,
    tool_file,
    Wrapper as Wrapper2File,
    UnWrapper as UnwrapperFile2Str
)
import                     time

class WorkflowStep(any_class):
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


