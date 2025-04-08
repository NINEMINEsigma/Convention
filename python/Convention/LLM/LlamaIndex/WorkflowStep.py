from Convention.Workflow.Core import Node
from .Core              import *
from ...Workflow.Core   import StepNode, StepNodeInfo, WorkflowActionWrapper, NodeSlotInfo

_Internal_WorkflowLLM:Dict[str, LLMObject] = {}
def AddLLM(name: str, llm: LLMObject):
    _Internal_WorkflowLLM[name] = llm
def RemoveLLM(name: str):
    _Internal_WorkflowLLM.pop(name)
def GetLLM(name: str) -> LLMObject:
    return _Internal_WorkflowLLM[name]
def ContainsLLM(name: str) -> bool:
    return name in _Internal_WorkflowLLM

_inject_llm_loader_funcname:Literal["__inject_llm_loader"] = "__inject_llm_loader"
def __inject_llm_loader(
    *,
    llm: str = "global",
    url_or_path: str = "",
    **kwargs
) -> LLMObject:
    if not ContainsLLM(llm):
        if WrapperFile(url_or_path).exists():
            AddLLM(llm, LLMObject.loading_LlamaCPP_from_local_path(url_or_path, **kwargs))
        else:
            AddLLM(llm, LLMObject.using_LlamaCPP_from_url(url_or_path, **kwargs))
    llm_object = GetLLM(llm)
    return {
        "llm": llm_object,
        "raw_llm": llm_object.ref_value,
    }

_inject_llm_loader_funcwrapper = WorkflowActionWrapper(_inject_llm_loader_funcname, __inject_llm_loader)

class LLMLoaderNode(StepNode):
    def __init__(self, info: 'LLMLoaderNodeInfo'):
        super().__init__(info)

class LLMLoaderNodeInfo(StepNodeInfo):
    '''
    加载LLM节点

    inmapping:
        llm_name: str
        url_or_path: str
    outmapping:
        llm: LLMObject
        raw_llm: LLM
    '''
    def __init__(
        self,
        *,
        title:              str             = "LLMLoader",
        llm_name_data:      Tuple[int, str] = (-1, "llm_name"),
        url_or_path_data:   Tuple[int, str] = (-1, "url_or_path"),
        outmapping_llm_nodeID:            int = -1,
        outmapping_raw_llm_nodeID:        int = -1,
        **kwargs
        ) -> None:
        '''
        inmapping:
            llm_name: str
            url_or_path: str
        outmapping:
            llm: LLMObject
            raw_llm: LLM

        参数 (nodeID, slotName):
            llm_name_data:      Tuple[int, str] = (-1, "llm_name")
            url_or_path_data:   Tuple[int, str] = (-1, "url_or_path")
            llm_data:           Tuple[int, str] = (-1, "llm")
            raw_llm_data:       Tuple[int, str] = (-1, "raw_llm")
        '''
        super().__init__(title=title, **kwargs)
        self.inmapping = {
            "llm_name": NodeSlotInfo(slotName="llm_name", typeIndicator="str", IsInmappingSlot=True,
                                     targetSlotName=llm_name_data[1], targetNodeID=llm_name_data[0]),
            "url_or_path": NodeSlotInfo(slotName="url_or_path", typeIndicator="str", IsInmappingSlot=True,
                                        targetSlotName=url_or_path_data[1], targetNodeID=url_or_path_data[0]),
        }
        self.outmapping = {
            "llm": NodeSlotInfo(slotName="llm", typeIndicator=LLMObject.__class__.__name__, IsInmappingSlot=False,
                               targetSlotName="llm", targetNodeID=outmapping_llm_nodeID),
            "raw_llm": NodeSlotInfo(slotName="raw_llm", typeIndicator=LLM.__class__.__name__, IsInmappingSlot=False,
                                    targetSlotName="raw_llm", targetNodeID=outmapping_raw_llm_nodeID),
        }
        self.funcname = _inject_llm_loader_funcname

    @override
    def Instantiate(self) -> Node:
        return LLMLoaderNode(self)

_inject_function_call_funcname:Literal["__inject_function_call"] = "__inject_function_call"
def __inject_function_call(
    *,
    llm:                        Optional[LLMObject] = None,
    function_tools:             List[FunctionTool]  = [],
    function_tools_str_forms:   Optional[str]       = None,
    **kwargs
) -> str:
    if llm is None:
        llm = LlamaIndexSettings.llm
    if function_tools_str_forms is None:
        pass
    else:
        pass

class FunctionCallNode(StepNode):
    def __init__(self, info: 'FunctionCallNodeInfo'):
        super().__init__(info)

class FunctionCallNodeInfo(StepNodeInfo):
    '''
    函数调用节点
    '''
    
