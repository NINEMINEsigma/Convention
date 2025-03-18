from ...Internal                         import *
from typing                                 import *
from langchain_core.language_models.base    import BaseMessage

from ...File.Core                        import tool_file

# Internal Definition

from langchain_core.tools                   import tool as FunctionTool
from langchain_core.language_models.base    import *

type MessageObject = LanguageModelInput
MessageType = Literal[
    "human",
    "user",
    "ai",
    "assistant",
    "system",
    "function",
    "tool"
    ]

# Abstract part Definition

class abs_llm_callable:
    @virtual
    def __call__(self, message:str) -> BaseMessage:
        return None

    def invoke(self, message:str) -> BaseMessage:
        return self(message)

class abs_llm_core(abs_llm_callable):
    @virtual
    def save_hestroy(self, file:Union[tool_file, str]):
        raise NotImplementedError()
    @virtual
    def load_hestory(self, file:Union[tool_file, str]):
        raise NotImplementedError()






