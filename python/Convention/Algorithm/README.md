# Algorithm 模块

Algorithm模块提供了多种算法实现，目前主要包含图算法和路径规划算法。

## 目录结构

- `Graph/`: 图算法相关实现
  - `Core.py`: 核心图算法实现
  - `LazyThetaStar.py`: Lazy Theta* 路径规划算法实现

## 功能特性

### 1. 图算法 (Graph)

#### 1.1 基础数据结构

- `GraphCore`: 图的核心数据结构，基于networkx实现
- `QuadTree`: 四叉树数据结构
- `Octree`: 八叉树数据结构

#### 1.2 图算法

- 最短路径算法
  - Dijkstra算法
  - Bellman-Ford算法
  - Floyd-Warshall算法
  - Johnson算法
- 最小生成树算法
  - Kruskal算法
  - Prim算法
  - Boruvka算法
- 图遍历算法
  - 深度优先搜索(DFS)
  - 广度优先搜索(BFS)
- 连通性分析
  - 连通分量
  - 强连通分量
  - 拓扑排序

#### 1.3 路径规划算法

- Lazy Theta* 算法
  - 2D路径规划
  - 3D路径规划
  - 支持四叉树和八叉树地图
  - 可视化功能

## 使用示例

### 1. 图算法使用示例

```python
from Convention.Algorithm.Graph import GraphCore

# 创建图
graph = GraphCore(is_directed=False)

# 添加节点
graph.add_node(1)
graph.add_node(2)
graph.add_node(3)

# 添加边
graph.add_edge(1, 2, weight=1.0)
graph.add_edge(2, 3, weight=2.0)
graph.add_edge(3, 1, weight=3.0)

# 计算最短路径
path = graph.dijkstra(1, 3)
```

### 2. 路径规划使用示例

```python
from Convention.Algorithm.Graph import LazyThetaStarPathFinder, Grid2D

# 创建2D网格
grid = Grid2D(10, 10)

# 设置障碍物
grid.set_obstacle(2, 2)
grid.set_obstacle(2, 3)

# 创建路径规划器
path_finder = LazyThetaStarPathFinder(grid)

# 寻找路径
start = (0, 0)
goal = (9, 9)
path = path_finder.find_path(start, goal)
```

## 依赖项

- networkx: 图算法基础库
- numpy: 数值计算
- OpenCV: 图像处理和可视化

## 注意事项

1. 使用图算法时，确保图的节点和边的数据结构正确
2. 路径规划算法需要合理设置网格大小和障碍物
3. 对于大规模图，建议使用更高效的算法实现

## 性能优化

1. 对于大规模图，可以使用稀疏矩阵表示
2. 路径规划时，可以通过调整网格大小来平衡精度和性能
3. 使用四叉树/八叉树可以加速空间查询

## 贡献指南

欢迎提交Issue和Pull Request来改进算法实现或添加新功能。
