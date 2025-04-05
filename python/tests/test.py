import time
import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

# from Convention.Algorithm.Graph.LazyThetaStar import *

# def main():
#     """主程序入口"""
#     # 创建可视化实例
#     vis = Visualization(width=1000, height=700, grid_size=40, cell_size=15)

#     # 运行可视化程序
#     vis.run()

from Convention.Lang.Reflection import *
from Convention.Lang.EasySave import *

class itest(BaseModel):
    _a:int = PrivateAttr(default=0)
class jtest(BaseModel):
    _b:int = PrivateAttr(default=0)
class ctest(itest, jtest):
    _c:int = PrivateAttr(default=0)

    d:str = Field(default="")
    e:int = Field(default=255)

    def test_invoke(self) -> str:
        return f"test_invoke: {self.d}-{self.e}"

    @staticmethod
    def test_invoke2(a:int, b:int) -> int:
        return a + b

    @classmethod
    def test_invoke3(cls, a:int, b:int) -> int:
        return a + b


def main():
    SetInternalDebug(True)
    from Convention.Workflow.Core import Workflow
    print(TypeManager.GetInstance().CreateOrGetRefType(Workflow).tree())

if __name__ == "__main__":
    main()