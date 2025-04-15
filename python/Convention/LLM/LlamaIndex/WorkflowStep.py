from typing import Coroutine, Dict
from .Core              import *
from ...Workflow.Core   import *

_Internal_WorkflowLLM:Dict[str, LLMObject] = {}
def AddLLM(name: str, llm: LLMObject):
    global _Internal_WorkflowLLM
    if len(_Internal_WorkflowLLM) == 0 and LlamaIndexSettings.llm is None:
        llm.set_as_global_llm()
    _Internal_WorkflowLLM[name] = llm
def RemoveLLM(name: str):
    global _Internal_WorkflowLLM
    _Internal_WorkflowLLM.pop(name)
def GetLLM(name: str) -> LLMObject:
    return _Internal_WorkflowLLM[name]
def ContainsLLM(name: str) -> bool:
    return name in _Internal_WorkflowLLM
def GetAllLLMNames() -> List[str]:
    return list(_Internal_WorkflowLLM.keys())

# region LLMLoader
def LLMLoader(llm_name:str, url_or_path:Optional[str] = None):
    if ContainsLLM(llm_name):
        llmObject = GetLLM(llm_name)
    elif url_or_path is None:
        llmObject = None
    elif url_or_path.startswith("http"):
        llmObject = LLMObject.using_LlamaCPP_from_url(url_or_path)
    else:
        llmObject = LLMObject.loading_LlamaCPP_from_local_path(url_or_path)
    return {
        "llm": llmObject,
        "raw_llm": llmObject.ref_value,
    }
_LLMLoader = WorkflowActionWrapper(LLMLoader.__name__, LLMLoader, "加载LLM",
                                   {"llm_name": "str", "url_or_path": "str"},
                                   {"llm": "LLMObject", "raw_llm": "LLM"})
# endregion

# region MakeGPTModelPrompt
def MakeGPTModelPrompt():
    from llama_index.core.prompts import ChatPromptTemplate
    return {
        "result": make_gpt_model_prompt(ChatPromptTemplate)
    }
_MakeGPTModelPrompt = WorkflowActionWrapper(MakeGPTModelPrompt.__name__, MakeGPTModelPrompt, "创建GPT模型提示",
                                           {},
                                           {"result": "Prompt"})
# endregion

# region EmbeddingLoader
def EmbeddingLoader(url:str, time_out:int = 120):
    return {
        "result": CustomEmbedding(url, time_out)
    }
_EmbeddingLoader = WorkflowActionWrapper(EmbeddingLoader.__name__, EmbeddingLoader, "加载Embedding",
                                        {"url": "str", "time_out": "int"},
                                        {"result": "Embedding"})
# endregion

# region KeywordQueryEngine
def MakeKeywordQueryEngine(
    retriever:      VectorIndexRetriever,
    keyword:        str,
    lang:           str = "zh"
    ):
    keyword_list = keyword.split(";")
    return {
        "result": make_query_engine_with_keywords(
            retriever,
            keyword_list,
            lang
        )
    }
_MakeKeywordQueryEngine = WorkflowActionWrapper(MakeKeywordQueryEngine.__name__, MakeKeywordQueryEngine, "创建关键词查询引擎",
                                               {"retriever": "VectorIndexRetriever", "keyword": "str", "lang": "str"},
                                               {"result": "QueryEngine"})
# endregion

# region MakeRetriever
def MakeRetriever(
        index:                      VectorStoreIndex,
        similarity_top_k:           int                         = DEFAULT_SIMILARITY_TOP_K,
        vector_store_query_mode:    VectorStoreQueryMode        = VectorStoreQueryMode.DEFAULT,
        filters:                    Optional[MetadataFilters]   = None,
        alpha:                      Optional[float]             = None,
        sparse_top_k:               Optional[int]               = None,
        hybrid_top_k:               Optional[int]               = None,
        embed_model:                Optional[BaseEmbedding]     = None,
        ):
    return {
        "result": make_retriever(
            index= index,
            similarity_top_k= similarity_top_k,
            vector_store_query_mode= vector_store_query_mode,
            filters= filters,
            alpha= alpha,
            sparse_top_k= sparse_top_k,
            hybrid_top_k= hybrid_top_k,
            embed_model= embed_model
        )
    }
_MakeRetriever = WorkflowActionWrapper(MakeRetriever.__name__, MakeRetriever, "创建Retriever",
                                        {
                                            "index": "VectorStoreIndex",
                                            "similarity_top_k": "int",
                                            "vector_store_query_mode": "VectorStoreQueryMode",
                                            "filters": "MetadataFilters",
                                            "alpha": "float",
                                            "sparse_top_k": "int",
                                            "hybrid_top_k": "int",
                                            "embed_model": "BaseEmbedding"
                                        },
                                        {"result": "VectorIndexRetriever"})
# endregion

# region FunctionToolLoader
_RegisteredFunctionTools:Dict[str, FunctionTool] = {}
def FunctionToolLoader(funcNames:  List[str|Any]|str = []):
    results = []
    if not isinstance(funcNames, list):
        funcNames = Unwrapper2Str(funcNames).split(";")
    for funcName_c in funcNames:
        funcName = Unwrapper2Str(funcName_c)
        if funcName in _RegisteredFunctionTools:
            results.append(_RegisteredFunctionTools[funcName])
        else:
            results.append(make_function_tool(funcName))
            _RegisteredFunctionTools[funcName] = results[-1]
    return {
        "result": results
    }
_FunctionToolLoader = WorkflowActionWrapper(FunctionToolLoader.__name__, FunctionToolLoader, "加载FunctionTool",
                                           {"funcNames": "Any"},
                                           {"result": "Array"})

def RegisterFunctionTool(funcName:str, func:Callable):
    _RegisteredFunctionTools[funcName] = func
def UnregisterFunctionTool(funcName:str):
    _RegisteredFunctionTools.pop(funcName)
def ContainsFunctionTool(funcName:str) -> bool:
    return funcName in _RegisteredFunctionTools
def GetAllFunctionToolNames() -> List[str]:
    return list(_RegisteredFunctionTools.keys())
#endregion

# region AgentLoader
def AgentLoader(
    tools: List[BaseTool] = [],
    llm: Optional[LLM] = None,
    ):
    return {
        "result": ReActAgentCore((tools, llm))
    }
_AgentLoader = WorkflowActionWrapper(AgentLoader.__name__, AgentLoader, "加载Agent",
                                     {"tools": "Array", "llm": "LLM"},
                                     {"result": "Agent"})
# endregion

# region Chat
def _SplitThinkingAndAnswer(message:str) -> Tuple[str, str]:
    if message.startswith("<think>"):
        thinking, _, answer = message.partition("</think>")
        if thinking.startswith("<thinking>"):
            thinking = thinking[len("<thinking>"):]
        return thinking.strip(), answer.strip()
    else:
        return "", message.strip()
def Chat(
    ai: ReActAgentCore|LLMObject|LLM,
    message: str,
    ):
    thinking:str = ""
    answer:str = ""
    if isinstance(ai, ReActAgentCore):
        thinking, answer = _SplitThinkingAndAnswer(ai.chat(message))
    elif isinstance(ai, LLMObject):
        thinking, answer = _SplitThinkingAndAnswer(ai.chat(message))
    elif isinstance(ai, LLM):
        thinking, answer = _SplitThinkingAndAnswer(ai.chat(message))
    return {
        "thinking": thinking,
        "answer": answer
    }
_Chat = WorkflowActionWrapper(Chat.__name__, Chat, "聊天",
                              {"ai": "Any", "message": "str"},
                              {"thinking": "str", "answer": "str"})
# endregion



