from ...Internal            import *
from .Core                  import EmbeddingCore
from ...Workflow.Step       import *
from llama_index.core       import PromptTemplate
from llama_index.core.llms  import LLM
from llama_index.core       import Settings
import                             json
import                             os
import                             time

def llm_step_execute(prompt_template:PromptTemplate, llm:Optional[LLM] = None, **kwargs) -> Dict[str, Any]:
    """
    执行LLM查询。
    参数:
        prompt_template: 提示模板
        llm: LLM模型实例
        **kwargs: 提示模板参数
    返回:
        LLM响应
    """
    if llm is None:
        llm = Settings.llm
    prompt = prompt_template.format(**kwargs)
    response = llm.complete(prompt)
    return {"result": response.text}

llm_step_action_wrapper = WorkflowActionWrapper("llm_step", llm_step_execute)

class LLMTextPromptTemplateResponseStep(WorkflowStep):
    """
    使用LLM执行的文本提示模板响应步骤。

    输入:
    ---

        prompt_template: 提示模板
        llm: LLM模型实例. 为空则

    输出:
    ---
        output_context_name: 指定输出到上下文的键值对中
    """

    def __init__(
        self,
        name:                           str,
        description:                    str,
        llm_context_name:               str                 = "llm",
        prompt_template_context_name:   str                 = "prompt_template",
        kwargs_context_name:            input_mapping_type  = {},
        output_context_name:            str                 = "result",
        retry_count:                    int                 = 3,
        retry_delay:                    int                 = 5,
    ):
        """
        初始化LLM步骤。

        参数:
            name: 步骤名称
            description: 步骤描述
            llm: LLM模型实例
            prompt_template: 提示模板
            input_mapping: 输入映射
            output_context_name: 输出上下文名称
            retry_count: 重试次数
            retry_delay: 重试延迟（秒）
        """
        self.llm_context_name = llm_context_name
        self.prompt_template_context_name = prompt_template_context_name
        self.kwargs_context_names = kwargs_context_name
        self.output_context_name = output_context_name

        super().__init__(
            name=name,
            description=description,
            action=llm_step_action_wrapper.name,
            input_mapping={
                "prompt_template": self.prompt_template_context_name,
                "llm": self.llm_context_name,
                **self.kwargs_context_names
            },
            output_mapping= {"result": self.output_context_name},
            retry_count=retry_count,
            retry_delay=retry_delay,
        )
