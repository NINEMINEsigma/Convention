from typing         import *
from .Core          import *
from ...Workflow    import *
from ...Workflow    import TextOperator

# region LLMLoader
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
        #vector_store_query_mode:    VectorStoreQueryMode        = VectorStoreQueryMode.DEFAULT,
        #filters:                    Optional[MetadataFilters]   = None,
        alpha:                      Optional[float]             = None,
        sparse_top_k:               Optional[int]               = None,
        hybrid_top_k:               Optional[int]               = None,
        #embed_model:                Optional[BaseEmbedding]     = None,
        ):
    return {
        "result": make_retriever(
            index= index,
            similarity_top_k= similarity_top_k,
            #vector_store_query_mode= vector_store_query_mode,
            #filters= filters,
            alpha= alpha,
            sparse_top_k= sparse_top_k,
            hybrid_top_k= hybrid_top_k,
            #embed_model= embed_model
        )
    }
_MakeRetriever = WorkflowActionWrapper(MakeRetriever.__name__, MakeRetriever, "创建Retriever",
                                        {
                                            "index": "VectorStoreIndex",
                                            "similarity_top_k": "int",
                                            #"vector_store_query_mode": "VectorStoreQueryMode",
                                            #"filters": "MetadataFilters",
                                            "alpha": "float",
                                            "sparse_top_k": "int",
                                            "hybrid_top_k": "int",
                                            #"embed_model": "BaseEmbedding"
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
def _SplitThinkingAndAnswer(response:ChatResponse) -> Tuple[str, str]:
    message = response.message.content
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
        thinking, answer = _SplitThinkingAndAnswer(ai.chat(make_user_message(message)))
    elif isinstance(ai, LLM):
        thinking, answer = _SplitThinkingAndAnswer(ai.chat(make_user_message(message)))
    return {
        "thinking": thinking,
        "answer": answer
    }
_Chat = WorkflowActionWrapper(Chat.__name__, Chat, "聊天",
                              {"ai": "Any", "message": "str"},
                              {"thinking": "str", "answer": "str"})
# endregion

# region KnowledgeDataBase

class KnowledgeDataBase:
    vectorStoreIndex:   IndexCore
    keywordTableIndex:  IndexCore
    summaryIndex:       IndexCore
    treeIndex:          IndexCore
    knowledgeGraphIndex:IndexCore

    def __init__(self,
                 vectorStoreIndex:  IndexCore,
                 keywordTableIndex: IndexCore,
                 summaryIndex:      IndexCore,
                 treeIndex:         IndexCore,
                 knowledgeGraphIndex:IndexCore
                 ) -> None:
        self.vectorStoreIndex = vectorStoreIndex
        self.keywordTableIndex = keywordTableIndex
        self.summaryIndex = summaryIndex
        self.treeIndex = treeIndex
        self.knowledgeGraphIndex = knowledgeGraphIndex


_Internal_WorkflowKnowledgeDataBase:Dict[str, KnowledgeDataBase] = {}
_Internal_KDBLoadingGuard: Dict[str, threading.Lock] = {}
'''
key is the name of the knowledge base
value is the path(or part of path) of the knowledge base
'''
def AddKDB(name: str,
           indexBuilder:        Optional[IndexBuilder|BaseReader|tool_file_or_str|Sequence[tool_file_or_str]],
           *,
           is_load_from_cache:  bool = False,
           cache_dir:           Optional[tool_file_or_str] = None
           ):
    _Internal_KDBLoadingGuard[name] = threading.Lock()
    with _Internal_KDBLoadingGuard[name]:
        global _Internal_WorkflowKnowledgeDataBase
        if is_load_from_cache:
            cache_dir = Wrapper2File(cache_dir)
            if cache_dir.exists() == False:
                raise ValueError(f"缓存目录不存在: {cache_dir}")
            vecDir = cache_dir|"vector_store_index/"
            vecDir.must_exists_path()
            keywordDir = cache_dir|"keyword_table_index/"
            keywordDir.must_exists_path()
            summaryDir = cache_dir|"summary_index/"
            summaryDir.must_exists_path()
            treeDir = cache_dir|"tree_index/"
            treeDir.must_exists_path()
            knowledgeGraphDir = cache_dir|"knowledge_graph_index/"
            knowledgeGraphDir.must_exists_path()
            _Internal_WorkflowKnowledgeDataBase[name] = KnowledgeDataBase(IndexCore((vecDir, "vector_store_index")),
                                                                          IndexCore((keywordDir, "keyword_table_index")),
                                                                          IndexCore((summaryDir, "summary_index")),
                                                                          IndexCore((treeDir, "tree_index")),
                                                                          IndexCore((knowledgeGraphDir, "knowledge_graph_index")))
        else:
            if not isinstance(indexBuilder, IndexBuilder):
                indexBuilder = IndexBuilder(indexBuilder)
            kdb = _Internal_WorkflowKnowledgeDataBase[name] = KnowledgeDataBase(indexBuilder.make_vector_store_index(),
                                                                          indexBuilder.make_keyword_table_index(),
                                                                          indexBuilder.make_summary_index(),
                                                                          indexBuilder.make_tree_index(),
                                                                          indexBuilder.make_knowledge_graph_index())
            if cache_dir is not None:
                cache_dir = Wrapper2File(cache_dir)
                cache_dir.must_exists_path()
                kdb.vectorStoreIndex.set_index_id("vector_store_index")
                kdb.vectorStoreIndex.save(cache_dir|"vector_store_index/")
                kdb.keywordTableIndex.set_index_id("keyword_table_index")
                kdb.keywordTableIndex.save(cache_dir|"keyword_table_index/")
                kdb.summaryIndex.set_index_id("summary_index")
                kdb.summaryIndex.save(cache_dir|"summary_index/")
                kdb.treeIndex.set_index_id("tree_index")
                kdb.treeIndex.save(cache_dir|"tree_index/")
                kdb.knowledgeGraphIndex.set_index_id("knowledge_graph_index")
                kdb.knowledgeGraphIndex.save(cache_dir|"knowledge_graph_index/")
def RemoveKDB(name: str):
    global _Internal_WorkflowKnowledgeDataBase
    if name in _Internal_KDBLoadingGuard:
        with lock_guard(_Internal_KDBLoadingGuard[name]):
            _Internal_WorkflowKnowledgeDataBase.pop(name)
def GetKDB(name: str) -> KnowledgeDataBase:
    global _Internal_WorkflowKnowledgeDataBase
    if name not in _Internal_KDBLoadingGuard:
        return None
    with lock_guard(_Internal_KDBLoadingGuard[name]):
        return _Internal_WorkflowKnowledgeDataBase[name]
def ContainsKDB(name: str) -> bool:
    global _Internal_WorkflowKnowledgeDataBase
    return name in _Internal_WorkflowKnowledgeDataBase
def GetAllKDBNames() -> List[str]:
    global _Internal_WorkflowKnowledgeDataBase
    return list(_Internal_WorkflowKnowledgeDataBase.keys())

def KnowledgeDataBaseLoader(
    name: str,
    data: Optional[IndexBuilder|BaseReader|tool_file_or_str|Sequence[tool_file_or_str]] = None
    ):
    if not ContainsKDB(name):
        AddKDB(name, data)
    kdb = GetKDB(name)
    return {
        "vectorStoreIndex": kdb.vectorStoreIndex,
        "keywordTableIndex": kdb.keywordTableIndex,
        "summaryIndex": kdb.summaryIndex,
        "treeIndex": kdb.treeIndex,
        "knowledgeGraphIndex": kdb.knowledgeGraphIndex
    }
_KnowledgeDataBaseLoader = WorkflowActionWrapper(KnowledgeDataBaseLoader.__name__, KnowledgeDataBaseLoader, "加载知识库",
                                                {"name": "str", "data": "Any"},
                                                {
                                                    "vectorStoreIndex": "IndexCore",
                                                    "keywordTableIndex": "IndexCore",
                                                    "summaryIndex": "IndexCore",
                                                    "treeIndex": "IndexCore",
                                                    "knowledgeGraphIndex": "IndexCore"
                                                })
def MakeQueryEngineTool(data:str|IndexCore):
    if isinstance(data, str):
        data = GetKDB(data).knowledgeGraphIndex
    return {
        "result": make_query_engine_tool(data)
    }
_MakeQueryEngineTool = WorkflowActionWrapper(MakeQueryEngineTool.__name__, MakeQueryEngineTool, "创建查询引擎工具",
                                            {"data": "Any"},
                                            {"result": "BaseTool"})
def MakeRouterQueryEngineTool(
    query_engine_tools: Sequence[QueryEngineTool],
    llm: Optional[LLM] = None,
    ):
    return {
        "result": make_router_query_engine_tool(query_engine_tools, llm)
    }
_MakeRouterQueryEngineTool = WorkflowActionWrapper(MakeRouterQueryEngineTool.__name__, MakeRouterQueryEngineTool, "创建路由查询引擎工具",
                                                {"query_engine_tools": "Array", "llm": "Any"},
                                                {"result": "BaseTool"})
def MakeSubQuestionQueryEngineTool(
    query_engine_tools: Sequence[QueryEngineTool],
    llm: Optional[LLM] = None,
    ):
    return {
        "result": make_sub_question_query_engine_tool(query_engine_tools, llm)
    }
_MakeSubQuestionQueryEngineTool = WorkflowActionWrapper(MakeSubQuestionQueryEngineTool.__name__, MakeSubQuestionQueryEngineTool, "创建子问题查询引擎工具",
                                                        {"query_engine_tools": "Array", "llm": "Any"},
                                                        {"result": "BaseTool"})
# endregion

# region IndexCore Building

def IndexCoreHandler(index:IndexCore):
    return {
        "result": index,
        "raw_index": index.ref_value
    }
_IndexCoreHandler = WorkflowActionWrapper(IndexCoreHandler.__name__, IndexCoreHandler, "处理IndexCore",
                                        {"index": "IndexCore"},
                                        {"result": "IndexCore", "raw_index": "Any"})

# endregion
