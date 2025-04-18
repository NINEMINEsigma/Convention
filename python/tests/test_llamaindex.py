import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from Convention.Lang.Core import *
from Convention.Lang.Reflection import *
from Convention.Workflow import *

async def run():
    try:
        SetInternalDebug(True)
        SetInternalReflectionDebug(True)
        SetInternalEasySaveDebug(True)
        SetInternalWorkflowDebug(True)
        AwakeBehaviorThread()
        manager = WorkflowManager.GetInstance()
        if True:
            manager.LoadWorkflow(r"C:\Users\000\Desktop\llm.json")
        else:
            manager.LoadWorkflow(Workflow.Create())
            node1 = manager.CreateNode(EndNodeInfo(title="结束", inmapping={
                "text": NodeSlotInfo(slotName="text", typeIndicator="str", IsInmappingSlot=True)
                }))
            node2 = manager.CreateNode(TextNodeInfo(text="你来自哪里", outmappingName="text"))
            NodeSlot.Link(node1.Internal_Inmapping["text"], node2.Internal_Outmapping["text"])
            manager.SaveWorkflow("./tests/test.json")
        await manager.RunWorkflow()
        print(manager.GetCurrentContext().ToString())
    except Exception as e:
        raise
    finally:
        StopBehaviorThread()
        print_colorful(ConsoleFrontColor.LIGHTWHITE_EX, TypeManager.GetInstance().AllRefTypes())


if __name__ == "__main__":
    run_until_complete(run())