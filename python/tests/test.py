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

def main():
    print(ValueInfo(int).__pydantic_private__)

if __name__ == "__main__":
    main()