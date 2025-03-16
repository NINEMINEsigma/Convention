from ...Internal            import *
from ...Workflow.Step       import *
from llama_index.core       import PromptTemplate
from llama_index.core.llms  import LLM

def llm_step_execute(prompt_template:PromptTemplate, llm:LLM, **kwargs) -> Dict[str, Any]:
    """
    执行LLM查询。
    参数:
        prompt_template: 提示模板
        llm: LLM模型实例
        **kwargs: 提示模板参数
    返回:
        LLM响应
    """
    prompt = prompt_template.format(**kwargs)
    response = llm.complete(prompt)
    return {"result": response.text}

llm_step_action_wrapper = WorkflowActionWrapper("llm_step", llm_step_execute)

class LLMTextPromptTemplateResponseStep(WorkflowStep):
    """
    LLM步骤类，表示使用LLM执行的工作流步骤。
    支持任何LLM模型。
    """

    def __init__(
        self,
        name:                   str,
        description:            str,
        llm_context_name:       str,
        prompt_template_name:   str,
        output_context_name:    str                 = None,
        retry_count:            int                 = 3,
        retry_delay:            int                 = 5,
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
        self.prompt_template_name = prompt_template_name
        self.output_context_name = output_context_name

        super().__init__(
            name=name,
            description=description,
            action=llm_step_action_wrapper.name,
            input_mapping={
                "llm": llm_context_name,
                "prompt_template": prompt_template_name,
            },
            output_mapping= {"result": output_context_name},
            retry_count=retry_count,
            retry_delay=retry_delay,
        )
