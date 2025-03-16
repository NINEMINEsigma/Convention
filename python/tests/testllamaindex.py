from ..Convention.Internal import *
from ..Convention.LLM.LlamaIndex.Agent import WorkflowAgent, LLM
from ..Convention.LLM.LlamaIndex.Workflow import Workflow, WorkflowStep, LLMTextPromptTemplateResponseStep
import os

def document_qa_example():
    """
    文档问答示例：使用工作流智能体从文档中回答问题
    """
    # 初始化工作流智能体
    agent = WorkflowAgent(
        documents="./documents",  # 文档目录
        model_path="/path/to/llama-model.gguf",  # LLM模型路径
        embedding_model_uid="embedding-model",  # 嵌入模型ID
        embedding_base_url="http://127.0.0.1:8080",  # 嵌入服务URL
    )

    # 查询文档
    response = agent.query("什么是LlamaIndex？")
    print(f"回答: {response}")

def custom_workflow_example():
    """
    自定义工作流示例：创建和执行自定义工作流
    """
    # 初始化LLM模型
    llm = LlamaCPP(
        model_path="/path/to/llama-model.gguf",
        temperature=0.1,
        max_new_tokens=512,
        context_window=4096,
        generate_kwargs={},
        model_kwargs={"n_gpu_layers": -1},
        verbose=True,
    )

    # 创建工作流
    workflow = Workflow(
        name="数据分析工作流",
        description="从数据中提取见解并生成报告"
    )

    # 添加数据加载步骤
    def load_data(file_path):
        print(f"加载数据: {file_path}")
        return {"data": f"从 {file_path} 加载的数据"}

    workflow.add_step(
        WorkflowStep(
            name="加载数据",
            description="从文件加载数据",
            action=load_data,
            input_mapping={"file_path": "input_file"},
            output_mapping={"data": "loaded_data"}
        )
    )

    # 添加数据分析步骤
    def analyze_data(data):
        print(f"分析数据: {data}")
        return {"analysis_result": f"{data} 的分析结果"}

    workflow.add_step(
        WorkflowStep(
            name="分析数据",
            description="分析加载的数据",
            action=analyze_data,
            input_mapping={"data": "loaded_data"},
            output_mapping={"analysis_result": "analysis_result"}
        )
    )

    # 添加LLM报告生成步骤
    workflow.add_step(
        LLMTextPromptTemplateResponseStep(
            name="生成报告",
            description="使用LLM生成分析报告",
            llm=llm,
            prompt_template="基于以下分析结果生成一份详细报告:\n\n{analysis_result}\n\n报告:",
            input_mapping={"analysis_result": "analysis_result"},
            output_mapping={"result": "final_report"}
        )
    )

    # 执行工作流
    result = workflow.execute({"input_file": "data.csv"})

    # 打印结果
    print(f"最终报告: {result.get('final_report', '未生成报告')}")

def rag_workflow_example():
    """
    RAG工作流示例：检索增强生成工作流
    """
    # 初始化工作流智能体
    agent = WorkflowAgent(
        documents="./documents",
        model_path="/path/to/llama-model.gguf",
        embedding_model_uid="embedding-model",
        embedding_base_url="http://127.0.0.1:8080",
    )

    # 初始化LLM模型
    llm = agent.llm

    # 创建RAG工作流
    workflow = Workflow(
        name="RAG工作流",
        description="检索增强生成工作流"
    )

    # 添加查询解析步骤
    workflow.add_step(
        LLMTextPromptTemplateResponseStep(
            name="查询解析",
            description="解析用户查询",
            llm=llm,
            prompt_template="请将以下用户查询分解为关键搜索词:\n\n{user_query}\n\n关键搜索词:",
            input_mapping={"user_query": "user_query"},
            output_mapping={"result": "search_keywords"}
        )
    )

    # 添加文档检索步骤
    def retrieve_documents(search_keywords):
        # 这里使用agent的查询功能
        print(f"使用关键词检索文档: {search_keywords}")
        return {"documents": f"使用 {search_keywords} 检索到的文档"}

    workflow.add_step(
        WorkflowStep(
            name="文档检索",
            description="检索相关文档",
            action=retrieve_documents,
            input_mapping={"search_keywords": "search_keywords"},
            output_mapping={"documents": "retrieved_documents"}
        )
    )

    # 添加回答生成步骤
    workflow.add_step(
        LLMTextPromptTemplateResponseStep(
            name="生成回答",
            description="基于检索到的文档生成回答",
            llm=llm,
            prompt_template="基于以下文档回答用户的问题:\n\n文档: {retrieved_documents}\n\n问题: {user_query}\n\n回答:",
            input_mapping={"retrieved_documents": "retrieved_documents", "user_query": "user_query"},
            output_mapping={"result": "final_answer"}
        )
    )

    # 执行工作流
    result = workflow.execute({"user_query": "LlamaIndex如何处理长文本？"})

    # 打印结果
    print(f"最终回答: {result.get('final_answer', '未生成回答')}")

if __name__ == "__main__":
    print("LlamaIndex工作流智能体示例")
    print("=" * 50)

    print("\n1. 文档问答示例")
    print("-" * 30)
    # document_qa_example()

    print("\n2. 自定义工作流示例")
    print("-" * 30)
    # custom_workflow_example()

    print("\n3. RAG工作流示例")
    print("-" * 30)
    # rag_workflow_example()