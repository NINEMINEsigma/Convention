from langchain_community.chat_models    import *
from typing                             import *
from ...Str.Core                     import UnWrapper
from ...File.Core                    import tool_file
from ...Lang.CppLike                 import *

from langchain_core.messages.ai             import AIMessage
from langchain_core.messages.tool           import ToolCall
from langchain_core.utils.function_calling  import convert_to_openai_tool
from langchain_core.prompts                 import ChatPromptTemplate
from langchain_core.tools.base              import BaseTool
from pydantic                               import BaseModel, Field

from ...LLM.LangChain.AbsInterface   import *

import random

# Make Defined

def do_make_content(**kwargs) -> MessageObject:
    if len(kwargs) == 2 and "role" in kwargs and "content" in kwargs:
        return (kwargs['role'], kwargs['content'])

    result:Dict[str,str]={}
    for key in kwargs:
        result[key] = str(kwargs[key])
    return result
def make_content(role:MessageType, content:str):
    return do_make_content(role=role, content=content)
def make_system_prompt(prompt:str):
    return make_content('system', prompt)
def make_human_prompt(message:str):
    return make_content('human', message)
def make_assistant_prompt(message:str):
    return make_content('assistant', message)

# Core Region

class light_llama_core(left_value_reference[ChatLlamaCpp], abs_llm_core):
    @property
    def model(self) -> ChatLlamaCpp:
        return self.ref_value
    @model.setter
    def model(self, value:ChatLlamaCpp):
        self.ref_value = value

    def __init__(self,
                 model:         Union[str, tool_file, ChatLlamaCpp],
                 init_message:  Union[MessageObject, List[MessageObject]]   = [],
                 temperature:   float                                       = 0.8,
                 is_record_result_to_history:bool                           = True,
                 *args, **kwargs):
        if init_message is None:
            init_message = []
        if isinstance(init_message, List) is False:
            if isinstance(init_message, str):
                init_message = make_system_prompt(init_message)
            else:
                init_message = [init_message]
        if isinstance(model, ChatLlamaCpp) is False:
            model = ChatLlamaCpp(
                model_path=UnWrapper(model),
                temperature=temperature,
                seed=random.randint(),
                *args,
                **kwargs)
        super().__init__(model)
        self.init_message_list      :List[MessageObject]    = init_message
        self.hestroy_message_list   :List[MessageObject]    = self.init_message_list.copy()
        self.last_result            :BaseMessage            = None
        self.is_record_result_to_history:bool               = is_record_result_to_history

    def __str__(self):
        return str(self.hestroy_message_list)

    def internal_call_with_recording(self, message:MessageObject):
        try:
            self.append_hestroy(message)
            self.last_result = self.model.invoke(self.hestroy_message_list)
            self.append_hestroy(make_assistant_prompt(self.last_result.content))
        except ValueError:
            self.pop_front_hestory()
            self.pop_hestroy()
            self.internal_call_with_recording(message)
        return self.last_result

    def internal_call_with_norecording(self, message:MessageObject):
        self.append_hestroy(message)
        result = self.model.invoke(self.hestroy_message_list)
        self.pop_hestroy()
        self.last_result = result
        return result

    @override
    def __call__(self, message:Union[str, MessageObject]) -> BaseMessage:
        message_object:MessageObject = make_human_prompt(message) if isinstance(message, str) else message
        if self.is_record_result_to_history:
            return self.internal_call_with_recording(message_object)
        else:
            return self.internal_call_with_norecording(message_object)

    def set_init_message(self, init_message:Union[MessageObject, List[MessageObject]]):
        self.clear_hestroy()
        if isinstance(init_message, List) is False:
            if isinstance(init_message, str):
                init_message = make_system_prompt(init_message)
            else:
                init_message = [init_message]
        self.init_message_list = init_message
        return self

    def pop_front_hestory(self):
        if len(self.hestroy_message_list) > len(self.init_message_list):
            self.hestroy_message_list.pop(len(self.init_message_list))
        return self
    def clear_hestroy(self):
        self.hestroy_message_list = self.init_message_list.copy()
        return self
    def pop_hestroy(self):
        result = self.hestroy_message_list.pop()
        return result
    def get_hestroy(self):
        return self.hestroy_message_list
    def set_hestroy(self, message:List[MessageObject]):
        self.hestroy_message_list = self.init_message_list.copy()
        self.append_hestroy(message)
    def append_hestroy(self, message:MessageObject):
        self.hestroy_message_list.append(message)
        return self
    @override
    def save_hestroy(self, file:Union[tool_file,str]):
        if isinstance(file, tool_file) is False:
            file = tool_file(UnWrapper(file))
            file.open('wb')
        file.data = self.hestroy_message_list
    @override
    def load_hestory(self, file:Union[tool_file,str]):
        if isinstance(file, tool_file) is False:
            file = tool_file(UnWrapper(file))
        if file.exists() is False:
            raise FileExistsError(f"{file.get_path()} is not found")
        file.open('rb')
        file.refresh()
        self.hestroy_message_list = file.data

    def get_last_message(self):
        return self.hestroy_message_list[-1]
    def append_message(self, message:MessageObject):
        self.hestroy_message_list.append(message)
        return self

    def bind_tools(
        self,
        tools: Sequence[Union[Dict[str, Any], Type[BaseModel], Callable, BaseTool]],
        *,
        tool_choice: Optional[Union[dict, bool, str]] = None,
        **kwargs: Any,
    ) -> Runnable[MessageObject, BaseMessage]:
        return self.model.bind_tools(tools, tool_choice=tool_choice, **kwargs)

    def with_structured_output(
        self,
        schema:         Union[Dict, Type[BaseModel]],
        include_raw:    bool                            = False
    ) -> Runnable[MessageObject, Union[Dict, BaseModel]]:
        return self.model.with_structured_output(schema, include_raw=include_raw)

    def with_stream_output(self, message:MessageObject, callback:Callable[[str], None]):
        for iter in self.model.stream(message):
            callback(iter)

def Wrapper(model:Union[str, tool_file]):
    return light_llama_core(UnWrapper(model))

# Tools Region

class runnable_llama_call(abs_llm_callable):
    def __init__(self, core:light_llama_core):
        self.core:  light_llama_core    = core

    @override
    def __call__(self, message:Union[str, MessageObject]) -> BaseMessage:
        return self.core(message)

def make_llama_call(core:light_llama_core):
    return runnable_llama_call(core)

class runnable_llama_prompt_call(Callable[[dict], BaseMessage]):
    def __init__(self, core:light_llama_core, prompt:ChatPromptTemplate):
        self.core:          light_llama_core                        = core
        self.prompt:        ChatPromptTemplate                      = prompt
        self.last_result:   BaseMessage                             = None
        self.chain:         RunnableSerializable[dict, BaseMessage] = self.prompt|self.core.model

    @override
    def __call__(self, message_inserter:dict, **kwargs) -> BaseMessage:
        #self.core.append_hestroy() ##what need to set with ai role
        combine_inserter:dict = { **message_inserter, **kwargs}
        self.last_result = self.chain.invoke(combine_inserter)
        self.core.append_hestroy(make_human_prompt(self.last_result.content))
        return self.last_result.content

class light_llama_prompt(Callable[[light_llama_core], runnable_llama_prompt_call]):
    '''
    prompt = ChatPromptTemplate.from_messages(
        [
            (
                "system",
                "You are a helpful assistant that translates {input_language} to {output_language}.",
            ),
            (
                "human",
                "{input}"
            ),
        ]
    )

    ...

    chain = prompt | llm
    chain.invoke(
        {
            "input_language": "English",
            "output_language": "German",
            "input": "I love programming.",
        }
    )
    '''
    def __init__(self, core:light_llama_core=None):
        self.prompt:    List[Tuple[str, str]]           = []
        self.core:      light_llama_core                = core

    def from_single_chat(self, role:MessageType, message_format:str):
        self.prompt.append((role, message_format))
        return self

    def append(self, role:MessageType, message_format:str, /):
        return self.from_single_chat(role, message_format)
    def __or__(self, role:MessageType, message_format:str, /):
        return self.from_single_chat(role, message_format)

    def s_append(self, message_format:str):
        return self.append("system", message_format)
    def h_append(self, message_format:str):
        return self.append("human", message_format)

    def __call__(self, core:light_llama_core=None) -> runnable_llama_prompt_call:
        if core is None:
            core = self.core
        if core is None:
            raise Exception("core is None")
        return runnable_llama_prompt_call(
            core,
            ChatPromptTemplate.from_messages(self.prompt)
            )

def make_llama_prompt_call(core:light_llama_core, prompts:Sequence[Tuple[MessageType, str]]):
    prompt = light_llama_prompt(core)
    for role, message_format in prompts:
        prompt.append(role, message_format)
    return prompt()

class runable_llama_tool_call_result(Callable[[], BaseMessage]):
    def __init__(self, tool_function, tool_calls_result:List[ToolCall]):
        self.tool_function:     Runnable[MessageObject, BaseMessage]    = tool_function
        self.tool_calls_result: List[ToolCall]                          = tool_calls_result

    def __call__(self) -> BaseMessage:
        return self.tool_function.invoke(self.tool_calls_result[0].args)

class runnable_llama_tool_call(Callable[[Union[str, MessageObject]], Any]):
    def __init__(
        self,
        core:           light_llama_core,
        tool_function:  Union[Dict[str, Any], Type[BaseModel], Callable, BaseTool],
        tool_choice:    str,
        **kwargs,
    ):
        self.last_result:   BaseMessage                                                 = None
        self.tool_function: Union[Dict[str, Any], Type[BaseModel], Callable, BaseTool]  = tool_function
        self.toolcall:      Runnable[MessageObject, BaseMessage]                        = core.bind_tools(
            [tool_function],
            tool_choice=self.make_tool_choice(tool_choice),
            **kwargs)

    def make_tool_choice(self, tool_choice_name:str):
        return {
            "type": "function",
            "function": {"name": tool_choice_name}
            }

    @override
    def __call__(self, message:Union[str, MessageObject]) -> BaseMessage:
        self.last_result:AIMessage = self.toolcall.invoke(message)
        return runable_llama_tool_call_result(self.tool_function, self.last_result.tool_calls)()

class light_llama_functioncall(Callable[[Union[str, MessageObject]], Sequence[Any]]):
    def __init__(
            self,
            core:           light_llama_core,
            tools:          Sequence[Runnable[MessageObject, BaseMessage]],
            tool_choice:    str = None,
            **kwargs: Any,
        ):
        self.last_tool_calls:   List[ToolCall]                                  = None
        self.core:              light_llama_core                                = core
        self.formatted_tools:   Dict[str, Runnable[MessageObject,BaseMessage]]  = {}
        self.target:            Runnable[MessageObject, BaseMessage]            = None
        for tool in tools:
            self.formatted_tools[convert_to_openai_tool(tool)["function"]["name"]] = tool
        self.bind_tools(tools, tool_choice=self.make_tool_choice(tool_choice), **kwargs)

    def make_tool_choice(self, tool_choice_name:str):
        return {
            "type": "function",
            "function": {"name": tool_choice_name}
            }

    def bind_tools(
            self,
            tools: Sequence[Union[Dict[str, Any], Type[BaseModel], Callable, BaseTool]],
            *,
            tool_choice: Optional[Union[dict, bool, str]] = None,
            **kwargs: Any,
    ) -> Runnable[MessageObject, BaseMessage]:
        self.target = self.core.bind_tools(tools, tool_choice=tool_choice, **kwargs)
        return self

    def __call__(self, message:Union[str, MessageObject]) -> Dict[str, Any]:
        self.last_ai_message:   AIMessage       = self.target.invoke(message)
        self.last_tool_calls:   List[ToolCall]  = self.last_ai_message.tool_calls
        self.last_result:Dict[str, Any]         = {}
        for tool_call in self.last_tool_calls:
            current_args:   dict[str, Any]  = tool_call['args']
            current_name:   str             = tool_call['name']
            if current_name in self.formatted_tools:
                self.last_result[current_name] = self.formatted_tools[current_name].invoke(current_args)
            else:
                raise Exception(f"Tool Function: {current_name} not found")
        return self.last_result

def make_llama_tool_call(
    core:               light_llama_core,
    tool_function:      Union[Dict[str, Any], Type[BaseModel], Callable, BaseTool],
    tool_choice_name:   str
    ) -> runnable_llama_tool_call:
    return runnable_llama_tool_call(core, tool_function, tool_choice_name)

def make_llama_functioncall(
    core:               light_llama_core,
    tool_functions:     Union[Runnable[MessageObject, BaseMessage], Sequence[Runnable[MessageObject, BaseMessage]]],
    tool_choice_name:   str = None,
    **kwargs: Any,
    ) -> light_llama_functioncall:
    return light_llama_functioncall(
        core,
        tool_functions if isinstance(tool_functions, Sequence) else [tool_functions],
        tool_choice=tool_choice_name,
        **kwargs)

class runnable_llama_structured_call(Callable[[Union[str, MessageObject]], Union[Dict, BaseModel]]):
    def __init__(
            self,
            core:           light_llama_core,
            schema:         Union[Dict, Type[BaseModel]],
            include_raw:    bool                            = False
        ):
        self.core:              light_llama_core                                = core
        self.structured_call:   Runnable[MessageObject, Union[Dict, BaseModel]] = core.with_structured_output(schema, include_raw)

    def __call__(self, message:Union[str, MessageObject]) -> Union[Dict, BaseModel]:
        return self.structured_call.invoke(message)

def make_llama_structured_call(
    core:               light_llama_core,
    schema:             Union[Dict, Type[BaseModel]],
    include_raw:        bool                            = False
    ) -> runnable_llama_structured_call:
    return runnable_llama_structured_call(core, schema, include_raw)

# Template Of FunctionCall Defined

class Internal_TemplateFunctionInput(BaseModel):
    function_input: int = Field(description="The input value for function")

@FunctionTool("get_function", args_schema=Internal_TemplateFunctionInput)
def magic_function(function_input: int):
    """Get the value of function for an input."""
    return function_input + 1

# LLM Chain
