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

class itest(BaseModel):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
    _a:int = PrivateAttr(default=0)
class jtest(BaseModel):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
    _b:int = PrivateAttr(default=0)
class ctest(itest, jtest):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
    _c:int = PrivateAttr(default=0)
    
    d:str = Field(default="")


def main():
    ctestxx = ctest()
    print_colorful(ConsoleFrontColor.RED, ctest.__pydantic_fields__)
    print_colorful(ConsoleFrontColor.YELLOW, ctestxx.model_dump())
    rtype = TypeManager.GetInstance().CreateOrGetRefType(ctest)
    print_colorful(ConsoleFrontColor.RED, rtype.GetFields(
        RefTypeFlag.Field|RefTypeFlag.Instance|RefTypeFlag.Private|RefTypeFlag.Public|RefTypeFlag.Special))
    


if __name__ == "__main__":
    main()