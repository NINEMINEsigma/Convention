import time
import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from Convention.LLM.LlamaIndex.Core import *

if __name__ == "__main__":
    llm = LLMObject.using_LlamaCPP_from_url("http://10.10.230.60:61111")
    llm.set_as_global_llm()
    print(llm.predict_and_call([], "你是谁"))






