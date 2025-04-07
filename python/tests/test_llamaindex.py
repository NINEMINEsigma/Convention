import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from Convention.Internal import *
from Convention.Workflow.Core import *
def query(user_msg:str) -> str:
    return f"query: {user_msg}"
query_wrapper = WorkflowActionWrapper("query", query)

async def run():
    print_colorful(ConsoleFrontColor.GREEN, "测试开始")
    try:
        manager = WorkflowManager.GetInstance()
        manager.LoadWorkflow(Workflow.Create(
            EndNodeInfo(inmapping={
                "answer": NodeSlotInfo(slotName="answer", targetNodeID=1, targetSlotName="answer", typeIndicator="str", IsInmappingSlot=True)
                }, nodeID=0),
            StepNodeInfo(funcname="query",
                         outmapping={
                             "answer": NodeSlotInfo(slotName="result", targetNodeID=0, targetSlotName="answer", typeIndicator="str", IsInmappingSlot=False)
                             },
                         inmapping={
                             "user_msg": NodeSlotInfo(slotName="user_msg", targetNodeID=2, targetSlotName="query", typeIndicator="str", IsInmappingSlot=True)
                             }, nodeID=1),
            TextNodeInfo(text="你是谁",
                         outmapping={
                             "query": NodeSlotInfo(slotName="query", targetNodeID=1, targetSlotName="user_msg", typeIndicator="str", IsInmappingSlot=True)
                             }, nodeID=2),
        ))
        await manager.RunWorkflow(verbose=True)
        print(manager.GetNode(0).end_result)
    except Exception:
        print_colorful(ConsoleFrontColor.RED, f"测试中断")
        raise
    finally:
        print_colorful(ConsoleFrontColor.GREEN, "测试结束")
        StopBehaviorThread()

if __name__ == "__main__":
    SetInternalDebug(True)
    SetInternalReflectionDebug(True)
    SetInternalEasySaveDebug(True)
    SetInternalWorkflowDebug(True)
    AwakeBehaviorThread()
    asyncio.run(run())

