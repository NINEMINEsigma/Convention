from .Step import *


class Workflow(left_value_reference[List[WorkflowStep]]):
    """
    工作流类，表示一系列步骤的执行流程。
    """

    def __init__(
        self,
        name:           str,
        description:    str,
        steps:          List[WorkflowStep] = None,
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
    def steps(self) -> List[WorkflowStep]:
        return self.ref_value

    def add_step(self, step: WorkflowStep) -> Self:
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
        self.steps = [WorkflowStep.Deserialize(step) for step in data["steps"]]
        return True

    @classmethod
    def Deserialize(cls, data: Dict[str, Any]) -> Self:
        """从字典反序列化工作流"""
        workflow = cls(data["name"], data["description"])
        workflow.steps = [WorkflowStep.Deserialize(step) for step in data["steps"]]
        return workflow
