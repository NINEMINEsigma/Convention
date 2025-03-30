from ...Internal                    import *
from .Core                          import *
from ...File.Core                   import tool_file, Wrapper as Wrapper2File, tool_file_or_str
from ...Str.Core                    import UnWrapper as UnWrapper2Str
from llama_index.core               import VectorStoreIndex, SimpleDirectoryReader, Settings, get_response_synthesizer
from llama_index.core.storage.storage_context import (
    StorageContext                  as     StorageContext
)
from llama_index.core.callbacks     import CallbackManager
from llama_index.core.schema        import TransformComponent
from llama_index.core.node_parser   import SentenceSplitter
from llama_index.core.retrievers    import VectorIndexRetriever
from llama_index.core.query_engine  import RetrieverQueryEngine
from llama_index.core.llms          import LLM
from llama_index.core.postprocessor import SimilarityPostprocessor
import os

# https://docs.llamaindex.ai/en/stable/module_guides/supporting_modules/service_context_migration/
class WorkflowAgent(any_class):
    """
    基于LlamaIndex的工作流智能体，用于构建和执行知识密集型工作流。
    支持本地模型。
    """

    embedding:          CustomEmbedding
    llm:                LLMObject
    storage_dir:        Wrapper2File
    reader:             SimpleDirectoryReader
    text_splitter:      SentenceSplitter
    storage_context:    StorageContext
    index_builder:      IndexBuilder
    index:              IndexCore

    def __init__(
        self,
        model_path:             Optional[str] = None,
        embedding:              Optional[CustomEmbedding|Tuple[str, str]] = None,
        storage_dir:            Optional[tool_file_or_str]              = None,
        chunk_size:             int                                     = 512,
        chunk_overlap:          int                                     = 50,
        **kwargs
    ):
        """
        初始化工作流智能体。

        参数:
            documents: 文档目录或文件路径
            model_path: LLM模型路径
            embedding: 嵌入模型或(ID, URL)
            storage_dir: 存储目录
            chunk_size: 文本分块大小
            chunk_overlap: 文本分块重叠大小
            **kwargs: 其他参数
        """
        # 初始化嵌入模型
        if embedding is None:
            self.embedding = CustomEmbedding.get_global_embedding()
        elif isinstance(embedding, CustomEmbedding):
            self.embedding = embedding
        else:
            self.embedding = CustomEmbedding(
                model_uid=embedding[0],
                base_url=embedding[1]
            )

        # 初始化LLM模型
        self.llm = LLMObject.loading_LlamaCPP_from_local_path(model_path)
        Settings.llm = self.llm

        # 创建文本分块器
        self.text_splitter = SentenceSplitter(
            chunk_size=chunk_size,
            chunk_overlap=chunk_overlap
        )

        # 创建存储上下文
        self.storage_context = StorageContext.from_defaults(
            persist_dir=UnWrapper2Str(self.storage_dir)
        )

        # 创建索引
        self.index_builder = IndexBuilder(self.reader)
        self.index = self.index_builder.build_vector_store_index(
            storage_context=self.storage_context,
            transformations=[self.text_splitter],
            show_progress=kwargs.get("show_progress", True)
        )

        # 创建检索器
        self.retriever = VectorIndexRetriever(
            index=self.index.index,
            similarity_top_k=kwargs.get("similarity_top_k", 3)
        )

        # 创建后处理器
        self.postprocessor = SimilarityPostprocessor(similarity_cutoff=kwargs.get("similarity_cutoff", 0.7))

        # 创建查询引擎
        self.query_engine = RetrieverQueryEngine(
            retriever=self.retriever,
            response_synthesizer=get_response_synthesizer(
                response_mode=kwargs.get("response_mode", "tree_summarize"),
                verbose=kwargs.get("verbose", True)
            ),
            node_postprocessors=[self.postprocessor]
        )

    def query(self, query_str: str) -> str:
        """
        执行查询。

        参数:
            query_str: 查询字符串
        返回:
            查询结果
        """
        response = self.query_engine.query(query_str)
        return str(response)

    def save_index(self) -> None:
        """
        保存索引到存储目录。
        """
        self.index.save(self.storage_dir)

    def load_index(self) -> None:
        """
        从存储目录加载索引。
        """
        self.index = IndexCore((self.storage_dir, "index"))
