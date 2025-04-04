import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from Convention.Internal import *

from Convention.LLM.LlamaIndex.Core import *
from Convention.LLM.LlamaIndex.Extensions import *
from Convention.Workflow.Core import *

llm = LLMObject(HttpLlamaCPP("http://10.10.230.60:61111", 120))
llm.set_as_global_llm()

def query(user_msg:str) -> str:
    print(f"query: {user_msg}")
    return llm.predict_and_call(user_msg, verbose=True)
query_wrapper = WorkflowActionWrapper("query", query)

async def run():
    print_colorful(ConsoleFrontColor.GREEN, "测试开始")
    try:
        manager = WorkflowManager.GetInstance()
        workflow = tool_file("./tests/test.json")
        if not workflow.exists():
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
            manager.SaveWorkflow("./tests/test.json")
        else:
            manager.LoadWorkflow(workflow)
        await manager.RunWorkflow(verbose=True)
        print_colorful(ConsoleFrontColor.GREEN, "测试结束")
        print(manager.GetNode[EndNode](0))
    except InterruptedError:
        print_colorful(ConsoleFrontColor.RED, "测试中断")
        raise
    except Exception as e:
        print_colorful(ConsoleFrontColor.RED, "测试中断")
        raise
    finally:
        StopBehaviorThread()

if __name__ == "__main__":
    SetInternalDebug(True)
    AwakeBehaviorThread()
    asyncio.run(run())

