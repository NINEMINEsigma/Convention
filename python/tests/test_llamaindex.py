import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from Convention.Internal import *

INTERNAL_DEBUG = True

from Convention.LLM.LlamaIndex.Core import *
from Convention.LLM.LlamaIndex.Extensions import *
from Convention.Workflow.Core import *

llm = LLMObject(HttpLlamaCPP("http://10.10.230.60:61111", 120))
llm.set_as_global_llm()

def run():
    manager = WorkflowManager.GetInstance()
    manager.LoadWorkflow(Workflow.CreateTemplate())
    manager.SaveWorkflow("./tests/test.json")

if __name__ == "__main__":
    run()

