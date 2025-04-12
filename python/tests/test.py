import time
import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from Convention.Lang.Core import *
from Convention.Lang.Reflection import *
from Convention.Lang.EasySave import *
from Convention.Workflow.Core import *

class A(BaseModel):
    a:int = Field(description="a")
class B(A):
    b:int = Field(description="b")
class D(B, any_class):
    d:int = Field(description="d")

def main():
    SetInternalDebug(True)
    SetInternalReflectionDebug(True)
    SetInternalEasySaveDebug(True)
    SetInternalWorkflowDebug(True)
    def error_handler(e:Exception):
        WorkflowManager.GetInstance().StopWorkflow()
        raise
    SetBehaviorExceptionHook(error_handler)
    AwakeBehaviorThread()
    WorkflowManager.GetInstance().LoadWorkflow("./tests/test.json")
    run_until_complete(WorkflowManager.GetInstance().RunWorkflow())
    StopBehaviorThread()

if __name__ == "__main__":
    main()



