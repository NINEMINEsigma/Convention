import pygame
import pygame.font
import time
import sys
import os

# 添加项目根目录到Python路径
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from Convention.Algorithm.Graph.LazyThetaStar import *

def main():
    """主程序入口"""
    # 创建可视化实例
    vis = Visualization(width=1000, height=700, grid_size=40, cell_size=15)

    # 运行可视化程序
    vis.run()

if __name__ == "__main__":
    main()