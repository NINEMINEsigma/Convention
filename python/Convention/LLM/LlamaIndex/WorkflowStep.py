from typing import Coroutine, Dict
from .Core              import *
from ...Workflow.Core   import *

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

class GlobalSettingNode(Node):
    def __init__(self, info: 'GlobalSettingNodeInfo'):
        super().__init__(info)

    @override
    async def _DoRunStep(self) -> Coroutine[Any, Any, Dict[str, Any] | Any]:
        parameters = await self.GetParameters()
        if "llm" in parameters:
            LlamaIndexSettings.llm = parameters["llm"]
        if "embed_model" in parameters:
            LlamaIndexSettings.embed_model = parameters["embed_model"]
        if "callback_manager" in parameters:
            LlamaIndexSettings.callback_manager = parameters["callback_manager"]
        if "chunk_overlap" in parameters:
            LlamaIndexSettings.chunk_overlap = parameters["chunk_overlap"]
        if "chunk_size" in parameters:
            LlamaIndexSettings.chunk_size = parameters["chunk_size"]
        if "context_window" in parameters:
            LlamaIndexSettings.context_window = parameters["context_window"]
        if "node_parser" in parameters:
            LlamaIndexSettings.node_parser = parameters["node_parser"]
        if "num_output" in parameters:
            LlamaIndexSettings.num_output = parameters["num_output"]
        if "prompt_helper" in parameters:
            LlamaIndexSettings.prompt_helper = parameters["prompt_helper"]
        if "text_splitter" in parameters:
            LlamaIndexSettings.text_splitter = parameters["text_splitter"]
        if "prompt_helper" in parameters:
            LlamaIndexSettings.prompt_helper = parameters["prompt_helper"]
        return {}

class GlobalSettingNodeInfo(NodeInfo):
    '''
    全局设置节点
    '''
    @override
    def Instantiate(self) -> GlobalSettingNode:
        return GlobalSettingNode(self)

__step_function_wrappers = [
    WorkflowActionWrapper(make_directory_reader.__name__, make_directory_reader),
    WorkflowActionWrapper(test_health.__name__, test_health),
    WorkflowActionWrapper(make_gpt_model_prompt.__name__, make_gpt_model_prompt),
    WorkflowActionWrapper(make_gpt_model_prompt_zh.__name__, make_gpt_model_prompt_zh),
    WorkflowActionWrapper(make_retriever.__name__, make_retriever),
    WorkflowActionWrapper(make_query_engine_with_keywords.__name__, make_query_engine_with_keywords),
    WorkflowActionWrapper(make_text_node.__name__, make_text_node),
    WorkflowActionWrapper(make_image_node_from_local_file.__name__, make_image_node_from_local_file),
    WorkflowActionWrapper(make_image_node_from_url.__name__, make_image_node_from_url),
    WorkflowActionWrapper(make_system_message.__name__, make_system_message),
    WorkflowActionWrapper(make_user_message.__name__, make_user_message),
    WorkflowActionWrapper(make_assistant_message.__name__, make_assistant_message),
]

class FunctionToolsLoaderNode(DynamicNode):
    def __init__(self, info: 'FunctionToolsLoaderNodeInfo'):
        super().__init__(info)

    @override
    async def _DoRunStep(self) -> Coroutine[Any, Any, Dict[str, Any] | Any]:
        parameters:Dict[str, Any] = await self.GetParameters()
        function_tools:Dict[str, FunctionTool] = {}
        for name, tool in parameters.items():
            if isinstance(tool, FunctionTool):
                function_tools[name] = tool
            elif isinstance(tool, Callable):
                function_tools[name] = make_function_tool(tool, name)
            elif isinstance(tool, str) and WorkflowActionWrapper.ContainsActionWrapper(tool):
                function_tools[name] = WorkflowActionWrapper.GetActionWrapper(tool)
            else:
                raise ValueError(f"Invalid tool: {tool}<{type(tool)}>")
        return function_tools

class FunctionToolsLoaderNodeInfo(DynamicNodeInfo):
    '''
    函数工具加载节点
    '''

    @override
    def Instantiate(self) -> FunctionToolsLoaderNode:
        return FunctionToolsLoaderNode(self)
