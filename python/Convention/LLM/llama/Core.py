from typing import *
from ...File.Core import tool_file
from llama_cpp import Llama, llama_grammar, llama
from llama_cpp.llama_types import *

MessageObject = ChatCompletionRequestMessage

class light_llama(Llama):
    def __init__(
        self,
        model_path: Union[str, tool_file],
        # Extra Params
        *args,
        **kwargs,  # type: ignore
    ):
        super().__init__(model_path=str(model_path), *args, **kwargs)
        self.function_pool:Dict[function] = None
        self.last_result = None
    def __del__(self):
        return super().__del__()
    def __str__(self):
        return self.last_result

    def predict_continuation(self, from_:str, *args, **kwargs):
        self.last_result =  super().__call__(from_, *args, **kwargs)
        return self.last_result

    def __call__(
        self,
        prompt:             str,
        suffix:             Optional[str]                           = None,
        max_tokens:         Optional[int]                           = 16,
        temperature:        float                                   = 0.8,
        top_p:              float                                   = 0.95,
        min_p:              float                                   = 0.05,
        typical_p:          float                                   = 1.0,
        logprobs:           Optional[int]                           = None,
        echo:               bool                                    = False,
        stop:               Optional[Union[str, List[str]]]         = [],
        frequency_penalty:  float                                   = 0.0,
        presence_penalty:   float                                   = 0.0,
        repeat_penalty:     float                                   = 1.0,
        top_k:              int                                     = 40,
        stream:             bool                                    = False,
        seed:               Optional[int]                           = None,
        tfs_z:              float                                   = 1.0,
        mirostat_mode:      int                                     = 0,
        mirostat_tau:       float                                   = 5.0,
        mirostat_eta:       float                                   = 0.1,
        model:              Optional[str]                           = None,
        stopping_criteria:  Optional[llama.StoppingCriteriaList]    = None,
        logits_processor:   Optional[llama.LogitsProcessorList]     = None,
        grammar:            Optional[llama_grammar.LlamaGrammar]    = None,
        logit_bias:         Optional[Dict[int, float]]              = None,
    ) -> Union[CreateCompletionResponse, Iterator[CreateCompletionStreamResponse]]:
        self.last_result = super().__call__(
            prompt=prompt,
            suffix=suffix,
            max_tokens=max_tokens,
            temperature=temperature,
            top_p=top_p,
            min_p=min_p,
            typical_p=typical_p,
            logprobs=logprobs,
            echo=echo,
            stop=stop,
            frequency_penalty=frequency_penalty,
            presence_penalty=presence_penalty,
            repeat_penalty=repeat_penalty,
            top_k=top_k,
            stream=stream,
            seed=seed,
            tfs_z=tfs_z,
            mirostat_mode=mirostat_mode,
            mirostat_tau=mirostat_tau,
            mirostat_eta=mirostat_eta,
            model=model,
            stopping_criteria=stopping_criteria,
            logits_processor=logits_processor,
            grammar=grammar,
            logit_bias=logit_bias,
        )
        return self.last_result

    def set_function_call(self, key:str, func:function):
        self.function_pool[key] = func
    def get_function_call(self, key:str):
        return self.function_pool[key]
    def call_function(self, key:str, **kwargs):
        return self.function_pool[key](**kwargs)
    def contains_function(self, key:str):
        return key in self.function_pool.keys()

    def get_last_result(self):
        return self.last_result


def do_make_content(**kwargs) -> MessageObject:
    result:MessageObject = {}
    for key,value in kwargs:
        result[key] = str(value)
    return result
def make_content(role:str, content:str):
    return do_make_content(role=role, content=content)
def make_system_content(prompt:str):
    return make_content("system", prompt)
def make_user_content(message:str):
    return make_content("user", message)
def make_assistant_content(message:str):
    return make_content("assistant", message)

class light_character:
    def __init__(self, core:light_llama, system_prompt:str=None):
        self._core:light_llama = core
        self.username:str = "user"

        if system_prompt is not None:
            self.__system_prompt:MessageObject = make_system_content(system_prompt)
            self._hestroy:List[MessageObject] = [self.__system_prompt]

    def clear_hestroy(self):
        self._hestroy.clear()
        return self
    def add_hestroy(self, message:MessageObject):
        self._hestroy.append(message)
        return self
    def add_user_hestroy(self, message:str):
        self.add_hestroy(make_user_content(message))
        return self
    def add_assistant_hestroy(self, message:str):
        self.add_hestroy(make_assistant_content(message))
        return self

    def create_chat_completion(
        self,
        message:            MessageObject,
        #functions:          Optional[List[ChatCompletionFunction]]          = None,
        #function_call:      Optional[ChatCompletionRequestFunctionCall]     = None,
        #tools:              Optional[List[ChatCompletionTool]]              = None,
        #tool_choice:        Optional[ChatCompletionToolChoiceOption]        = None,
        temperature:        float                                           = 0.2,
        top_p:              float                                           = 0.95,
        top_k:              int                                             = 40,
        min_p:              float                                           = 0.05,
        typical_p:          float                                           = 1.0,
        stream:             bool                                            = False,
        stop:               Optional[Union[str, List[str]]]                 = [],
        seed:               Optional[int]                                   = None,
        response_format:    Optional[ChatCompletionRequestResponseFormat]   = None,
        max_tokens:         Optional[int]                                   = None,
        presence_penalty:   float                                           = 0.0,
        frequency_penalty:  float                                           = 0.0,
        repeat_penalty:     float                                           = 1.0,
        tfs_z:              float                                           = 1.0,
        mirostat_mode:      int                                             = 0,
        mirostat_tau:       float                                           = 5.0,
        mirostat_eta:       float                                           = 0.1,
        model:              Optional[str]                                   = None,
        logits_processor:   Optional[llama.LogitsProcessorList]             = None,
        grammar:            Optional[llama_grammar.LlamaGrammar]            = None,
        logit_bias:         Optional[Dict[int, float]]                      = None,
        logprobs:           Optional[bool]                                  = None,
        top_logprobs:       Optional[int]                                   = None,
    ) -> Union[
        CreateChatCompletionResponse, Iterator[CreateChatCompletionStreamResponse]
    ]:
        self.add_hestroy(message)
        self.last_result = self._core.create_chat_completion(
            messages=self._hestroy,
            temperature=temperature,
            top_p=top_p,
            top_k=top_k,
            min_p=min_p,
            typical_p=typical_p,
            stream=stream,
            stop=stop,
            seed=seed,
            response_format=response_format,
            max_tokens=max_tokens,
            presence_penalty=presence_penalty,
            frequency_penalty=frequency_penalty,
            repeat_penalty=repeat_penalty,
            tfs_z=tfs_z,
            mirostat_mode=mirostat_mode,
            mirostat_tau=mirostat_tau,
            mirostat_eta=mirostat_eta,
            model=model,
            logits_processor=logits_processor,
            grammar=grammar,
            logit_bias=logit_bias,
            logprobs=logprobs,
            top_logprobs=top_logprobs,
        )
        #TODO
        return self.last_result

if __name__ == '__main__':
    model = light_llama(r'D:\LLM\MODELs\llama3-8B\Meta-Llama-3-8B-Instruct\Meta-Llama-3-8B-Instruct-Q4_0.gguf')
    print(model.create_chat_completion(
        messages=[{
            "role": "user",
            "content": "what is the meaning of life?"
        }]
    ))


