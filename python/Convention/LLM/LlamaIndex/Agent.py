from ...Internal                    import *
from .Core                          import LlamaCPPEmbedding, make_directory_reader
from ...File.Core                   import tool_file, Wrapper as Wrapper2File, tool_file_or_str
from ...Str.Core                    import UnWrapper as UnWrapper2Str
from llama_index.core               import VectorStoreIndex, SimpleDirectoryReader, Settings
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

    def __init__(
        self,
        documents:          tool_file_or_str,
        llm:                LLM,
        embedding_model:    Optional[LlamaCPPEmbedding] = None,
        chunk_size:         int = 1024,
        chunk_overlap:      int = 20,
        similarity_top_k:   int = 3,
    ):
        """
        初始化工作流智能体。

        参数:
            documents: 文档目录路径或文档路径
            llm: 语言模型实例
            embedding_model: 嵌入模型实例，如果为None则使用默认嵌入模型
            chunk_size: 文档分块大小
            chunk_overlap: 文档分块重叠大小
            similarity_top_k: 检索时返回的最相似节点数量
        """
        self.documents:         tool_file_or_str            = Wrapper2File(documents)
        self.llm:               LLM                         = llm
        self.embedding_model:   Optional[LlamaCPPEmbedding] = embedding_model
        self.chunk_size:        int                         = chunk_size
        self.chunk_overlap:     int                         = chunk_overlap
        self.similarity_top_k:  int                         = similarity_top_k

        # 配置全局设置
        settings = Settings.from_defaults(
            llm=self.llm,
            embed_model=self.embedding_model,
            node_parser=SentenceSplitter(
                chunk_size=chunk_size,
                chunk_overlap=chunk_overlap
            )
        )

        # 加载文档并创建索引
        self.index = None
        self.load_documents(settings)

    def load_documents(
        self,
        storage_context:        Optional[StorageContext]            = None,
        show_progress:          bool                                = False,
        callback_manager:       Optional[CallbackManager]           = None,
        transformations:        Optional[List[TransformComponent]]  = None,
        **kwargs: Any,
        ) -> VectorStoreIndex:
        """
        加载文档并创建索引

        参数:
            storage_context: 可选的StorageContext对象，用于配置索引创建
            show_progress: 是否显示进度条
            callback_manager: 可选的CallbackManager对象，用于管理回调
            transformations: 可选的TransformComponent对象列表，用于配置索引创建
            **kwargs: 其他可选参数
        """
        if not self.documents.exists():
            print(f"文档目录 {self.documents} 不存在")
            return

        try:
            # 加载文档
            documents =  make_directory_reader(self.documents).load_data()
            print(f"已加载 {len(documents)} 个文档")

            self.index = VectorStoreIndex.from_documents(
                documents,
                storage_context=storage_context,
                show_progress=show_progress,
                callback_manager=callback_manager,
                transformations=transformations,
                **kwargs
            )
            print("索引创建成功")
        except Exception as e:
            print(f"加载文档或创建索引时出错: {str(e)}")
        return self.index

    def query(self, query_text: str) -> str:
        """
        查询知识库并返回回答。

        参数:
            query_text: 查询文本
        返回:
            回答文本
        """
        if self.index is None:
            return "索引尚未创建，请先加载文档"

        try:
            # 创建检索器
            retriever = VectorIndexRetriever(
                index=self.index,
                similarity_top_k=self.similarity_top_k,
            )

            # 创建后处理器
            postprocessor = SimilarityPostprocessor(similarity_cutoff=0.7)

            # 创建查询引擎，显式传递llm参数
            query_engine = RetrieverQueryEngine(
                retriever=retriever,
                node_postprocessors=[postprocessor],
                llm=self.llm
            )

            # 执行查询
            response = query_engine.query(query_text)
            return str(response)
        except Exception as e:
            return f"查询时出错: {str(e)}"

    def create_workflow(self, workflow_name: str, workflow_steps: List[Dict[str, Any]]):
        """
        创建工作流。

        参数:
            workflow_name: 工作流名称
            workflow_steps: 工作流步骤列表，每个步骤是一个字典
        """
        # 这里可以实现工作流的创建和存储逻辑
        pass

    def execute_workflow(self, workflow_name: str, input_data: Dict[str, Any] = None):
        """
        执行工作流。

        参数:
            workflow_name: 工作流名称
            input_data: 输入数据
        """
        # 这里可以实现工作流的执行逻辑
        pass

    @classmethod
    def class_name(cls) -> str:
        """返回类名"""
        return "WorkflowAgent"