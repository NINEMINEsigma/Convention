from ...Internal                                import *
from ...Str.Core                                import UnWrapper
from ...File.Core                               import tool_file_or_str, UnWrapper as UnwrapperFile2Str
from pydantic                                   import Field
import requests                                 as     requests
import asyncio                                  as     asyncio
import aiohttp                                  as     aiohttp
from llama_index.core                           import (
    SimpleDirectoryReader                       as     SimpleDirectoryReader,
    Settings                                    as     LlamaIndexSettings,
    VectorStoreIndex                            as     VectorStoreIndex,
    KeywordTableIndex                           as     KeywordTableIndex,
    load_index_from_storage                     as     load_index_from_storage,
    get_response_synthesizer                    as     get_response_synthesizer,
    )
from llama_index.core.node_parser               import SentenceSplitter
from llama_index.core.llms                      import (
    LLM                                         as     LLM,
    ChatMessage                                 as     ChatMessage,
    CompletionResponse                          as     CompletionResponse,
    ChatResponse                                as     ChatResponse,
    MessageRole                                 as     MessageRole,
    )
from llama_index.core.embeddings                import BaseEmbedding
from llama_index.core.schema                    import (
    Document                                    as     Document,
    TransformComponent                          as     TransformComponent,
    )
from llama_index.core.storage                   import StorageContext
from llama_index.core.callbacks                 import CallbackManager
from llama_index.core.query_engine              import CustomQueryEngine
from llama_index.core.retrievers                import BaseRetriever
from llama_index.core.response_synthesizers     import BaseSynthesizer
from llama_index.core.agent                     import ReActAgent
from llama_index.core.tools                     import FunctionTool
from llama_index.core.types                     import TokenGen

from llama_index.core.prompts.base              import BasePromptTemplate
from llama_index.core.indices.base              import BaseIndex
from llama_index.core.readers.base              import BaseReader
from llama_index.core.base.base_query_engine    import BaseQueryEngine
from llama_index.core.base.response.schema      import RESPONSE_TYPE

# https://zhuanlan.zhihu.com/p/16349452850
# Prompt -- Reader -- Index -- Retriever -- Query Engine -- Agent --Workflow

# SimpleDirectoryReader用于从目录中读取文档
# https://docs.llamaindex.ai/en/stable/module_guides/loading/simpledirectoryreader/#simpledirectoryreader
def make_directory_reader(
    input_data:             Union[tool_file_or_str, List[tool_file_or_str]],
    exclude_data:           Optional[List[str]]                 = None,
    exclude_hidden:         bool                                = True,
    exclude_empty:          bool                                = False,
    errors:                 str                                 = "ignore",
    recursive:              bool                                = False,
    encoding:               str                                 = "utf-8",
    filename_as_id:         bool                                = False,
    required_extensions:    Optional[list[str]]                 = None,
    num_files_limit:        Optional[int]                       = None,
    file_metadata:          Optional[Callable[[str], dict]]     = None,
    raise_on_error:         bool                                = False,
    **kwargs
    ) -> SimpleDirectoryReader:
    '''
    """
    创建SimpleDirectoryReader实例。

    参数:
        input_data: 输入数据，可以是单个文件/目录路径或路径列表
        exclude: 要排除的文件glob模式列表
        exclude_hidden: 是否排除隐藏文件
        exclude_empty: 是否排除空文件
        errors: 编码错误处理方式
        recursive: 是否递归搜索子目录
        encoding: 文件编码
        filename_as_id: 是否使用文件名作为文档ID
        required_exts: 必需的文件扩展名列表
        num_files_limit: 读取文件数量限制
        file_metadata: 文件元数据提取函数
        raise_on_error: 读取失败时是否抛出异常
        kwargs: 其他参数

    返回:
        SimpleDirectoryReader实例
    """
    '''
    input_dir = None
    input_files = None
    if isinstance(input_data, tool_file_or_str):
        if input_data.is_dir():
            input_dir = input_data
        else:
            input_files = [input_data]
    elif isinstance(input_data, list):
        input_files = [file for file in input_data if isinstance(file, tool_file_or_str)]

    for i in range(len(required_extensions)):
        if required_extensions[i].startswith("."):
            required_extensions[i] = required_extensions[i][1:]

    return SimpleDirectoryReader(
        input_dir=input_dir,
        input_files= input_files,
        exclude=exclude_data,
        exclude_hidden=exclude_hidden,
        exclude_empty=exclude_empty,
        errors=errors,
        recursive=recursive,
        encoding=encoding,
        filename_as_id=filename_as_id,
        required_exts=required_extensions,
        num_files_limit=num_files_limit,
        file_metadata=file_metadata,
        raise_on_error=raise_on_error,
        **kwargs
    )

def test_health(url:str) -> int:
    response = requests.get(f"{url}/health")
    return response.status_code

# Prompt Layer - 提示词模板层
# 用于构建查询和响应的提示词模板
# https://zhuanlan.zhihu.com/p/16350943831
'''
from llama_index.core import ChatPromptTemplate
from llama_index.core.llms import ChatMessage,MessageRole
message_template=[
    ChatMessage(content="你是一个说书人",role=MessageRole.SYSTEM),
    ChatMessage(content="给我讲讲{topic}？",role=MessageRole.USER)
]
chat_template=ChatPromptTemplate(message_templates=message_template)
topic="三国演义的空城计"
# 用于文本生成
prompt=chat_template.format(topic=topic)
print(prompt)
print('----------')
# 用于聊天
message=chat_template.format_messages(topic=topic)
print(message)
'''
def make_gpt_model_prompt[T:BasePromptTemplate](cls:Typen[T]) -> T:
    return cls(
        "Context information is below.\n"
        "---------------------\n"
        "{context_str}\n"
        "---------------------\n"
        "Given the context information and not prior knowledge, "
        "answer the query.\n"
        "Query: {query_str}\n"
        "Answer: "
    )
def make_gpt_model_prompt_zh[T:BasePromptTemplate](cls:Typen[T]) -> T:
    return cls(
        "以下是上下文信息：\n"
        "---------------------\n"
        "{context_str}\n"
        "---------------------\n"
        "仅基于上述上下文信息，不使用任何先验知识，"
        "回答如下问题：\n"
        "问题：{query_str}\n"
        "回答："
    )
# End Layer

# Query Engine Layer - 查询引擎层
#https://docs.llamaindex.ai/en/stable/examples/query_engine/custom_query_engine/
class AbsCustomQueryEngine(CustomQueryEngine,ABC):
    """自定义查询引擎的抽象基类"""
    pass
class RAGQueryEngine(AbsCustomQueryEngine):
    """
    RAG(检索增强生成)查询引擎

    使用检索器获取相关文档,然后使用响应合成器生成答案
    """

    retriever: BaseRetriever
    response_synthesizer: BaseSynthesizer

    def custom_query(self, query_str: str) -> RESPONSE_TYPE:
        """
        执行RAG查询

        参数:
            query_str: 查询字符串

        返回:
            生成的响应
        """
        nodes = self.retriever.retrieve(query_str)
        response_obj = self.response_synthesizer.synthesize(query_str, nodes)
        return response_obj
class RAGStringQueryEngine(AbsCustomQueryEngine):
    """
    RAG字符串查询引擎

    使用检索器获取相关文档,然后直接使用LLM生成字符串答案
    """

    retriever:              BaseRetriever
    response_synthesizer:   BaseSynthesizer
    llm:                    LLM
    qa_prompt:              BasePromptTemplate

    def custom_query(self, query_str: str):
        """
        执行RAG字符串查询

        参数:
            query_str: 查询字符串

        返回:
            生成的字符串响应
        """
        nodes = self.retriever.retrieve(query_str)

        context_str = "\n\n".join([n.node.get_content() for n in nodes])
        response = self.llm.complete(
            self.qa_prompt.format(context_str=context_str, query_str=query_str)
        )

        return str(response)
# End Layer

# Index Layer And Reader Layer - 索引和读取器层
class IndexCore[IndexType:BaseIndex](left_value_reference[IndexType]):
    """
    索引核心类，用于管理索引相关的操作。
    """
    def __init__(
        self,
        index:      IndexType|Tuple[StorageContext|tool_file_or_str, str]
        ):
        if isinstance(index, BaseIndex):
            super().__init__(index)
        else:
            if isinstance(index[0], StorageContext):
                super().__init__(load_index_from_storage(index[0], index[1]))
            else:
                super().__init__(load_index_from_storage(
                    StorageContext.from_defaults(persist_dir=UnWrapper(index[0])), index[1]
                    ))

    @property
    def index(self) -> IndexType:
        return self.ref_value

    def set_index_id(self, index_id: str) -> None:
        """设置索引ID"""
        self.index.set_index_id(index_id)
    def get_index_id(self) -> str:
        """获取索引ID"""
        return self.index.index_id

    def save(self, dirpath:tool_file_or_str) -> None:
        """
        保存索引到指定目录

        参数:
            dirpath: 保存目录路径
        """
        self.index.storage_context.persist(UnWrapper(dirpath))

    __query_engine: BaseQueryEngine = None
    def rebuild_query_engine(
        self,
        llm:            Optional[LLM]   = None,
        **kwargs,
        ):
        '''
        response_mode:      str             = "tree_summarize"
        text_qa_template:   PromptTemplate  = QA_PROMPT
        '''
        self.__query_engine = self.index.as_query_engine(
            llm=llm,
            **kwargs
            )
    @property
    def query_engine(self) -> BaseQueryEngine:
        if self.__query_engine is None:
            self.rebuild_query_engine()
        return self.__query_engine
    def query(self, query: str) -> RESPONSE_TYPE:
        """
        执行查询

        参数:
            query: 查询字符串

        返回:
            查询响应
        """
        return self.query_engine.query(query)
    @property
    def query_prompt(self) -> Dict[str, BasePromptTemplate]:
        """获取查询提示词模板"""
        return self.query_engine.get_prompts()
    @query_prompt.setter
    def query_prompt(self, value: Dict[str, BasePromptTemplate]) -> None:
        """设置查询提示词模板"""
        self.query_engine.update_prompts(value)
class IndexBuilder[Reader:BaseReader](left_value_reference[Reader]):
    """
    索引构建器类,用于创建索引

    主要功能:
    - 从Reader加载文档
    - 构建不同类型的索引(向量存储索引、关键词表索引等)
    """

    __documents: List[Document] = None

    def __init__(
        self,
        reader_or_documentsDir:     Reader|tool_file_or_str,
        /,
        reader_cls:               Type[Reader] = SimpleDirectoryReader,
        ):
        super().__init__(
            reader_or_documentsDir
            if isinstance(reader_or_documentsDir, reader_cls)
            else reader_cls(UnwrapperFile2Str(reader_or_documentsDir))
            )

    @property
    def reader(self) -> Reader:
        return self.ref_value

    def __load_documents(self):
        self.__documents = self.reader.load_data()

    @property
    def documents(self) -> List[Document]:
        if self.__documents is None:
            self.__load_documents()
        return self.__documents
    @documents.setter
    def documents(self, value: List[Document]):
        self.__documents = value
    def reset_documents(self):
        self.__load_documents()

    def build_vector_store_index(
        self,
        storage_context:    Optional[StorageContext]            = None,
        show_progress:      bool                                = False,
        callback_manager:   Optional[CallbackManager]           = None,
        transformations:    Optional[List[TransformComponent]]  = None,
        **kwargs,
        ) -> IndexCore[VectorStoreIndex]:
        """
        构建向量存储索引

        参数:
            storage_context: 存储上下文
            show_progress: 是否显示进度
            callback_manager: 回调管理器
            transformations: 文档转换组件列表
            kwargs: 其他参数

        返回:
            向量存储索引实例
        """
        return IndexCore[VectorStoreIndex](VectorStoreIndex.from_documents(
            self.documents,
            storage_context=storage_context,
            show_progress=show_progress,
            callback_manager=callback_manager,
            transformations=transformations,
            **kwargs,
        ))
    def build_keyword_table_index(
        self,
        storage_context:    Optional[StorageContext]            = None,
        show_progress:      bool                                = False,
        callback_manager:   Optional[CallbackManager]           = None,
        transformations:    Optional[List[TransformComponent]]  = None,
        **kwargs
        ) -> IndexCore[KeywordTableIndex]:
        """
        构建关键词表索引

        参数:
            storage_context: 存储上下文
            show_progress: 是否显示进度
            callback_manager: 回调管理器
            transformations: 文档转换组件列表
            kwargs: 其他参数

        返回:
            关键词表索引实例
        """
        return IndexCore[KeywordTableIndex](KeywordTableIndex.from_documents(
            self.documents,
            storage_context=storage_context,
            show_progress=show_progress,
            callback_manager=callback_manager,
            transformations=transformations,
            **kwargs,
            ))
# End Layer

# Embedding Layer - 嵌入层
class EmbeddingCore(BaseEmbedding, any_class):
    """
    LlamaCPP嵌入类,用于生成文本嵌入向量

    主要功能:
    - 生成文本的嵌入向量
    - 支持同步和异步操作
    - 支持批量处理
    - 可设置为全局嵌入模型
    """

    model_uid:  str     = Field(
        default="unknown",
        description="要使用的LlamaCPP模型ID(或文件路径)。",
    )
    base_url:   str     = Field(
        default="http://127.0.0.1:8080",
        description="LlamaCPP服务的基础URL。",
    )
    timeout:    float   = Field(
        default=60.0,
        description="请求超时时间(秒)。",
    )

    def set_as_global_embedding(self) -> None:
        LlamaIndexSettings.embed_model = self
    @classmethod
    def get_global_embedding(cls) -> BaseEmbedding:
        return LlamaIndexSettings.embed_model

    def __init__(
        self,
        model_uid: str,
        base_url: str = "http://127.0.0.1:8080",
        timeout: float = 60.0,
        **kwargs: Any,
    ) -> None:
        """
        初始化LlamaCPP嵌入类

        参数:
            model_uid: LlamaCPP模型ID(或文件路径)
            base_url: LlamaCPP服务URL,默认为http://127.0.0.1:8080
            timeout: 请求超时时间,默认60秒
            **kwargs: 其他参数
        """
        super().__init__(
            model_uid=model_uid,
            base_url=base_url,
            timeout=timeout,
            **kwargs,
        )

    @classmethod
    def class_name(cls) -> str:
        """返回类名"""
        return "LlamaCPPEmbedding"

    def _get_query_embedding(self, query: str) -> List[float]:
        """
        获取查询文本的嵌入向量。

        参数:
            query: 查询文本
        返回:
            嵌入向量列表
        """
        return self.get_general_text_embedding(query)

    async def _aget_query_embedding(self, query: str) -> List[float]:
        """
        异步获取查询文本的嵌入向量。

        参数:
            query: 查询文本
        返回:
            嵌入向量列表
        """
        return await self.aget_general_text_embedding(query)

    def _get_text_embedding(self, text: str) -> List[float]:
        """
        获取文本的嵌入向量。

        参数:
            text: 输入文本
        返回:
            嵌入向量列表
        """
        return self.get_general_text_embedding(text)

    async def _aget_text_embedding(self, text: str) -> List[float]:
        """
        异步获取文本的嵌入向量。

        参数:
            text: 输入文本
        返回:
            嵌入向量列表
        """
        return await self.aget_general_text_embedding(text)

    def _get_text_embeddings(self, texts: List[str]) -> List[List[float]]:
        """
        批量获取多个文本的嵌入向量。

        参数:
            texts: 文本列表
        返回:
            嵌入向量列表的列表
        """
        embeddings_list: List[List[float]] = []
        for text in texts:
            embeddings = self.get_general_text_embedding(text)
            embeddings_list.append(embeddings)
        return embeddings_list

    async def _aget_text_embeddings(self, texts: List[str]) -> List[List[float]]:
        """
        异步批量获取多个文本的嵌入向量。

        参数:
            texts: 文本列表
        返回:
            嵌入向量列表的列表
        """
        return await asyncio.gather(
            *[self.aget_general_text_embedding(text) for text in texts]
        )

    def get_general_text_embedding(self, prompt: str) -> List[float]:
        """
        调用LlamaCPP服务获取文本嵌入向量。

        参数:
            prompt: 输入文本
        返回:
            嵌入向量列表
        异常:
            如果API调用失败则抛出异常
        """
        headers = {"Content-Type": "application/json"}
        json_data = {"input": prompt, "model": self.model_uid}
        try:
            response = requests.post(
                url=f"{self.base_url}/embedding",
                headers=headers,
                json=json_data,
                timeout=self.timeout,
            )
            response.encoding = "utf-8"
            if response.status_code != 200:
                raise Exception(
                    f"LlamaCPP call failed with status code {response.status_code}."
                    f"Details: {response.text}"
                )

            # 打印响应内容以便调试
            response_json = response.json()
            if globals().get("__DEBUG__", False):
                print(f"API响应: {response_json}")
            return response_json["data"]["embedding"]
        except Exception as e:
            print(f"获取嵌入向量时出错: {str(e)}")
            raise

    async def aget_general_text_embedding(self, prompt: str) -> List[float]:
        """
        异步调用LlamaCPP服务获取文本嵌入向量。

        参数:
            prompt: 输入文本
        返回:
            嵌入向量列表
        异常:
            如果API调用失败则抛出异常
        """
        headers = {"Content-Type": "application/json"}
        json_data = {"input": prompt, "model": self.model_uid}
        try:
            async with aiohttp.ClientSession() as session:
                async with session.post(
                    url=f"{self.base_url}/embedding",
                    headers=headers,
                    json=json_data,
                    timeout=self.timeout,
                ) as response:
                    if response.status != 200:
                        raise Exception(
                            f"LlamaCPP call failed with status code {response.status}."
                        )

                    data = await response.json()
                    # 打印响应内容以便调试
                    if globals().get("__DEBUG__", False):
                        print(f"异步API响应: {data}")
                    return data["data"]["embedding"]
        except Exception as e:
            print(f"异步获取嵌入向量时出错: {str(e)}")
            raise
# End Layer

# LLM Object Layer
def make_system_message(content:str) -> ChatMessage:
    return ChatMessage(role=MessageRole.SYSTEM, content=content)
def make_user_message(content:str) -> ChatMessage:
    return ChatMessage(role=MessageRole.USER, content=content)
def make_assistant_message(content:str) -> ChatMessage:
    return ChatMessage(role=MessageRole.ASSISTANT, content=content)
class LLMObject(left_value_reference[LLM]):
    """
    LLM对象类，用于管理LLM对象。
    """
    def __init__(self, llm:LLM) -> None:
        super().__init__(llm)

    @property
    def llm(self) -> LLM:
        return self.ref_value
    @llm.setter
    def llm(self, value:LLM) -> None:
        self.ref_value = value

    def set_as_global_llm(self) -> None:
        LlamaIndexSettings.llm = self.llm
    @classmethod
    def get_global_llm(cls) -> LLM:
        return LlamaIndexSettings.llm

    def complete(self, prompt:str, **kwargs:Any) -> CompletionResponse:
        '''
        文本完成
        ---
        如果LLM是一个聊天模型,提示词会被转换为单个`user`消息。

        参数:
            prompt: str
                发送给LLM的提示词。
            formatted: bool, optional
                提示词是否已经为LLM格式化,默认为False。
            kwargs: Any
                传递给LLM的额外关键字参数。

        返回:
            CompletionResponse: LLM的补全响应。
                text: str
                    存储模型生成的文本内容, 在非流式响应中存储完整文本, 在流式响应中存储当前已生成的文本部分
                additional_kwargs: dict
                    存储额外的响应信息, 例如：token 计数、函数调用信息等, 使用 Field(default_factory=dict) 确保默认值为空字典
                raw: Optional[Any]
                    可选的原始 JSON 数据, 用于存储解析前的原始响应数据
                logprobs: Optional[List[List[LogProb]]]
                    可选的 token 概率信息, 用于存储每个生成 token 的对数概率
                delta: Optional[str]
                    仅在流式响应中使用, 存储新生成的文本片段

        示例:
            ```python
            response = llm.complete("你的提示词")
            print(response.text)
            ```
        '''
        return self.llm.complete(prompt, **kwargs)
    def stream(self, prompt:str, **kwargs:Any) -> TokenGen:
        return self.llm.stream(prompt, **kwargs)
    def chat(self, *messages:ChatMessage, **kwargs:Any) -> ChatResponse:
        """
        聊天
        ---
        参数:
            messages: Sequence[ChatMessage]
                聊天消息序列。
            kwargs: Any
                传递给LLM的额外关键字参数。

        返回:
            ChatResponse: LLM的聊天响应。
                message: ChatMessage
                    这是最核心的字段, 存储完整的聊天消息内容, 类型是 ChatMessage，包含了消息的角色、内容等信息
                raw: Optional[Any]
                    可选的原始响应数据, 用于存储未处理的原始响应
                delta: Optional[str]
                    用于流式响应, 存储新生成的文本片段
                logprobs: Optional[List[List[LogProb]]]
                    可选的 token 概率信息, 用于存储生成过程中每个 token 的对数概率
                additional_kwargs: dict
                    存储额外的响应信息, 使用 Field(default_factory=dict) 确保默认值为空字典

        示例:
            ```python
            from llama_index.core.llms import ChatMessage

            response = llm.chat([ChatMessage(role="user", content="你好")])
            print(response.content)
            ```
        """
        return self.llm.chat(messages, **kwargs)


    @classmethod
    def create_LlamaCPP_from_local_path(cls, model_path:tool_file_or_str, **kwargs:Any) -> Self:
        from llama_index.llms.llama_cpp     import LlamaCPP
        return cls(LlamaCPP(model_path=UnWrapper(model_path), **kwargs))
    @classmethod
    def create_LlamaCPP_from_url(cls, model_url:str, **kwargs:Any) -> Self:
        from llama_index.llms.llama_cpp     import LlamaCPP
        return cls(LlamaCPP(base_url=model_url, **kwargs))

    @classmethod
    def create_OpenAI_from_api_key(cls, api_key:str, **kwargs:Any) -> Self:
        from llama_index.llms.openai        import OpenAI
        return cls(OpenAI(api_key=api_key, **kwargs))
    @classmethod
    def create_OpenAI_from_api_key_file(cls, api_key_file:tool_file_or_str, **kwargs:Any) -> Self:
        from llama_index.llms.openai        import OpenAI
        return cls(OpenAI(api_key=UnWrapper(api_key_file), **kwargs))
# End Layer

if __name__ == "__main__":
    embedding = EmbeddingCore(model_uid="", base_url="http://127.0.0.1:61111")
    embedding.set_as_global_embedding()

    embedding.get_general_text_embedding("你好")







