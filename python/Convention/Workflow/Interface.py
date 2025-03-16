from ..Internal import *


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

