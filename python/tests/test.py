import time
import sys
import os

from Convention.Algorithm.Graph.LazyThetaStar import *

def main():
    """主程序入口"""
    # 创建可视化实例
    vis = Visualization(width=1000, height=700, grid_size=40, cell_size=15)

    # 运行可视化程序
    vis.run()

if __name__ == "__main__":
    main()