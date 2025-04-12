import time
import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from Convention.Lang.Core import *
from Convention.Lang.Reflection import *
from Convention.Lang.EasySave import *
from Convention.Workflow.Core import *

def query(user_msg:str):
    return "<query>:"+user_msg

query_action = WorkflowActionWrapper("query", query)

def main():
    SetInternalDebug(True)
    SetInternalReflectionDebug(True)
    SetInternalEasySaveDebug(True)
    SetInternalWorkflowDebug(True)
    def error_handler(e:Exception):
        # 停止工作流并传递错误信息
        WorkflowManager.GetInstance().StopWorkflow(error=e)
        # 不再立即抛出异常，让正常清理流程执行
        print(f"捕获到异常: {e}")
    SetBehaviorExceptionHook(error_handler)
    AwakeBehaviorThread()
    
    try:
        WorkflowManager.GetInstance().LoadWorkflow("./tests/test.json")
        WorkflowManager.GetInstance().set_timeout(60)
        try:
            run_until_complete(WorkflowManager.GetInstance().RunWorkflow(True))
            run_until_complete(WorkflowManager.GetInstance().RunWorkflow(True))
        except Exception as e:
            print(f"工作流执行过程中发生错误: {e}")
        print(WorkflowManager.GetInstance().GetCurrentContext())
    finally:
        # 确保行为线程始终被停止，防止程序无法退出
        StopBehaviorThread()
        print("行为线程已停止")
    
    print(TypeManager.GetInstance().AllRefTypes())

if __name__ == "__main__":
    main()



