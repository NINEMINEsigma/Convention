import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from Convention.Internal import *
from Convention.Lang.Core import *
from Convention.Workflow.Core import *
from Convention.LLM.LlamaIndex.WorkflowStep import *
from Convention.LLM.LlamaIndex.Core import LLMObject

def query(llm:LLMObject, user_msg:str) -> str:
    #return llm.predict_and_call([], user_msg)
    return ReActAgentCore(([], llm.ref_value), True).chat(user_msg, False)
query_wrapper = WorkflowActionWrapper("query", query)

async def run(query:str, model:str):
    print_colorful(ConsoleFrontColor.GREEN, "测试开始")
    manager = WorkflowManager.GetInstance()
    try:
        manager.LoadWorkflow(Workflow.Create(
            EndNodeInfo(inmapping={
                "answer": NodeSlotInfo(slotName="answer",
                                       targetNodeID=1,
                                       targetSlotName="answer",
                                       typeIndicator="str",
                                       IsInmappingSlot=True)
                }, nodeID=0),
            StepNodeInfo(funcname="query",
                         outmapping={
                             "answer": NodeSlotInfo(slotName="answer",
                                                    targetNodeID=0,
                                                    targetSlotName="answer",
                                                    typeIndicator="str",
                                                    IsInmappingSlot=False)
                             },
                         inmapping={
                             "llm": NodeSlotInfo(slotName="llm",
                                                 targetNodeID=3,
                                                 targetSlotName="llm",
                                                 typeIndicator=LLMObject.__class__.__name__,
                                                 IsInmappingSlot=True),
                             "user_msg": NodeSlotInfo(slotName="user_msg",
                                                      targetNodeID=2,
                                                      targetSlotName="query",
                                                      typeIndicator="str",
                                                      IsInmappingSlot=True)
                             }, nodeID=1),
            TextNodeInfo(text=query, outmappingName="query", nodeID=2, targetNodeID=1, targetSlotName="user_msg"),
            StepNodeInfo(funcname=LLMLoader.__name__,
                         inmapping={
                             "llm_name": "str", "url_or_path": "str"},
                         outmapping=
                                   
                                   {"llm": "LLMObject", "raw_llm": "LLM"}
                         , nodeID=3),
            TextNodeInfo(text="llm", outmappingName="llm_name", targetNodeID=3, targetSlotName="llm_name", nodeID=4),
            TextNodeInfo(text=model, outmappingName="url_or_path", targetNodeID=3, targetSlotName="url_or_path", nodeID=5),
        is_auto_id=False))
        WorkflowManager.GetInstance().SaveWorkflow("./tests/test.json")
        return
        await manager.RunWorkflow(verbose=True)
        print_colorful(ConsoleFrontColor.WHITE, f"测试结果: {manager.GetNode(0).end_result}")
    except Exception:
        print_colorful(ConsoleFrontColor.RED, f"测试中断")
        raise
    finally:
        print_colorful(ConsoleFrontColor.GREEN, "测试结束")
    return manager.GetNode(0).end_result

if __name__ == "__main__":
    #SetInternalDebug(True)
    #SetInternalReflectionDebug(True)
    #SetInternalEasySaveDebug(True)
    #SetInternalWorkflowDebug(True)
    #def error_handler(e:Exception):
    #    WorkflowManager.GetInstance().StopWorkflow()
    #    raise
    #SetBehaviorExceptionHook(error_handler)
    #AwakeBehaviorThread()
    run_until_complete(run("你是谁", "主模型"))
    #StopBehaviorThread()