import time
import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from Convention.LLM.LlamaIndex.Core import *

def get_content():
    return "You are designed to help with a variety of tasks, from answering questions to providing summaries to other types of analyses."
tools = [
    make_sync_func_tool(get_content)
]

if __name__ == "__main__":
    llm = LLMObject.using_LlamaCPP_from_url("http://10.10.230.60:61111")
    llm.set_as_global_llm()
    agent = ReActAgentCore((tools,llm.ref_value), True)
    print_colorful(ConsoleFrontColor.GREEN, agent.get_prompt())
    agent.update_prompts({
        "agent_worker:system_prompt": make_gpt_model_prompt_zh(PromptTemplate)
    })
    print_colorful(ConsoleFrontColor.RED, agent.get_prompt())
    print_colorful(ConsoleFrontColor.GREEN, agent.chat("你是谁"))






