import heapq
import math
import numpy as np
from ...Internal import *
from .Core import QuadTree, Octree, GraphCore

class Vector3D(any_class):
    """表示3D空间中的一个点"""
    def __init__(self, x: float, y: float, z: float = 0):
        self.x = x
        self.y = y
        self.z = z

    def __eq__(self, other):
        if isinstance(other, Vector3D):
            return self.x == other.x and self.y == other.y and self.z == other.z
        return False

    def __hash__(self):
        return hash((self.x, self.y, self.z))

    def distance_to(self, other: Self) -> float:
        """计算到另一个点的欧几里得距离"""
        return math.sqrt((self.x - other.x) ** 2 + (self.y - other.y) ** 2 + (self.z - other.z) ** 2)

    def __str__(self):
        return f"({self.x}, {self.y}, {self.z})"

    def to_tuple(self) -> Tuple[float, float, float]:
        """转换为元组表示"""
        return (self.x, self.y, self.z)

class Vector2D(any_class):
    """表示2D空间中的一个点"""
    def __init__(self, x: float, y: float):
        self.x = x
        self.y = y

    def __eq__(self, other: Self) -> bool:
        if isinstance(other, Vector2D):
            return self.x == other.x and self.y == other.y
        return False

    def __hash__(self):
        return hash((self.x, self.y))

    def distance_to(self, other: Self) -> float:
        return math.sqrt((self.x - other.x) ** 2 + (self.y - other.y) ** 2)

    def __str__(self):
        return f"({self.x}, {self.y})"

    def to_tuple(self) -> Tuple[float, float]:
        """转换为元组表示"""
        return (self.x, self.y)

class PathNode(any_class):
    """表示路径规划中的一个节点"""
    def __init__(self, position: Vector3D|Vector2D):
        self.position = position
        self.parent = None
        self.g = float('inf')  # 从起点到当前节点的实际代价
        self.h = 0             # 从当前节点到终点的估计代价

    @property
    def f(self):
        """f值 = g值 + h值"""
        return self.g + self.h

class Grid2D(any_class):
    """2D网格表示"""
    def __init__(self, width: int, height: int):
        self.width = width
        self.height = height
        self.obstacles = np.zeros((width, height), dtype=bool)

    def set_obstacle(self, x: int, y: int, is_obstacle: bool = True):
        """设置指定位置是否为障碍物"""
        if self.is_valid_position(x, y):
            self.obstacles[x, y] = is_obstacle

    def is_obstacle(self, x: int, y: int) -> bool:
        """检查指定位置是否为障碍物或无效位置"""
        return not self.is_valid_position(x, y) or self.obstacles[x, y]

    def is_obstacle_vector(self, position: Vector2D) -> bool:
        """检查Vector2位置是否为障碍物"""
        return self.is_obstacle(position.x, position.y)

    def is_valid_position(self, x: int, y: int) -> bool:
        """检查位置是否在网格范围内"""
        return 0 <= x < self.width and 0 <= y < self.height

    def is_valid_position_vector(self, position: Vector2D) -> bool:
        """检查Vector2位置是否在网格范围内"""
        return self.is_valid_position(position.x, position.y)

    def line_of_sight(self, start: Vector2D, end: Vector2D) -> bool:
        """检查从起点到终点是否有直线视野（无障碍物阻挡）"""
        x0, y0 = start.x, start.y
        x1, y1 = end.x, end.y

        dx = abs(x1 - x0)
        dy = abs(y1 - y0)
        sx = 1 if x0 < x1 else -1
        sy = 1 if y0 < y1 else -1
        err = dx - dy

        while x0 != x1 or y0 != y1:
            if self.is_obstacle(x0, y0):
                return False

            e2 = 2 * err
            if e2 > -dy:
                err -= dy
                x0 += sx
            if e2 < dx:
                err += dx
                y0 += sy

        return not self.is_obstacle(x1, y1)

    def get_neighbors(self, position: Vector2D) -> List[Vector2D]:
        """获取指定位置的相邻节点（8个方向）"""
        neighbors = []

        for dx in [-1, 0, 1]:
            for dy in [-1, 0, 1]:
                if dx == 0 and dy == 0:
                    continue

                new_x = position.x + dx
                new_y = position.y + dy

                if self.is_valid_position(new_x, new_y) and not self.is_obstacle(new_x, new_y):
                    neighbors.append(Vector2D(new_x, new_y))

        return neighbors

    def clear(self):
        """清除所有障碍物"""
        self.obstacles.fill(False)

class Grid3D(any_class):
    """3D网格表示"""
    def __init__(self, width: int, height: int, depth: int):
        self.width = width
        self.height = height
        self.depth = depth
        self.obstacles = np.zeros((width, height, depth), dtype=bool)
    def set_obstacle(self, x: int, y: int, z: int, is_obstacle: bool = True):
        """设置指定位置是否为障碍物"""
        if self.is_valid_position(x, y, z):
            self.obstacles[x, y, z] = is_obstacle

    def is_obstacle(self, x: int, y: int, z: int) -> bool:
        """检查指定位置是否为障碍物或无效位置"""
        return not self.is_valid_position(x, y, z) or self.obstacles[x, y, z]

    def is_obstacle_vector(self, position: Vector3D) -> bool:
        """检查Vector3D位置是否为障碍物"""
        return self.is_obstacle(position.x, position.y, position.z)

    def is_valid_position(self, x: int, y: int, z: int) -> bool:
        """检查位置是否在网格范围内"""
        return 0 <= x < self.width and 0 <= y < self.height and 0 <= z < self.depth

    def is_valid_position_vector(self, position: Vector3D) -> bool:
        """检查Vector3D位置是否在网格范围内"""
        return self.is_valid_position(position.x, position.y, position.z)

    def line_of_sight(self, start: Vector3D, end: Vector3D) -> bool:
        """检查从起点到终点是否有直线视野（无障碍物阻挡）"""
        x0, y0, z0 = start.x, start.y, start.z
        x1, y1, z1 = end.x, end.y, end.z

        dx = abs(x1 - x0)
        dy = abs(y1 - y0)
        dz = abs(z1 - z0)
        sx = 1 if x0 < x1 else -1
        sy = 1 if y0 < y1 else -1
        sz = 1 if z0 < z1 else -1

        if dx >= dy and dx >= dz:
            err_1 = 2 * dy - dx
            err_2 = 2 * dz - dx
            while x0 != x1:
                if self.is_obstacle(x0, y0, z0):
                    return False
                if err_1 > 0:
                    y0 += sy
                    err_1 -= 2 * dx
                if err_2 > 0:
                    z0 += sz
                    err_2 -= 2 * dx
                err_1 += 2 * dy
                err_2 += 2 * dz
                x0 += sx
        elif dy >= dx and dy >= dz:
            err_1 = 2 * dx - dy
            err_2 = 2 * dz - dy
            while y0 != y1:
                if self.is_obstacle(x0, y0, z0):
                    return False
                if err_1 > 0:
                    x0 += sx
                    err_1 -= 2 * dy
                if err_2 > 0:
                    z0 += sz
                    err_2 -= 2 * dy
                err_1 += 2 * dx
                err_2 += 2 * dz
                y0 += sy
        else:
            err_1 = 2 * dy - dz
            err_2 = 2 * dx - dz
            while z0 != z1:
                if self.is_obstacle(x0, y0, z0):
                    return False
                if err_1 > 0:
                    y0 += sy
                    err_1 -= 2 * dz
                if err_2 > 0:
                    x0 += sx
                    err_2 -= 2 * dz
                err_1 += 2 * dy
                err_2 += 2 * dx
                z0 += sz

        return not self.is_obstacle(x1, y1, z1)

    def get_neighbors(self, position: Vector3D) -> List[Vector3D]:
        """获取指定位置的相邻节点（26个方向）"""
        neighbors = []

        for dx in [-1, 0, 1]:
            for dy in [-1, 0, 1]:
                for dz in [-1, 0, 1]:
                    if dx == 0 and dy == 0 and dz == 0:
                        continue

                    new_x = position.x + dx
                    new_y = position.y + dy
                    new_z = position.z + dz

                    if self.is_valid_position(new_x, new_y, new_z) and not self.is_obstacle(new_x, new_y, new_z):
                        neighbors.append(Vector3D(new_x, new_y, new_z))

        return neighbors

    def clear(self):
        """清除所有障碍物"""
        self.obstacles.fill(False)

class PriorityQueue[T](any_class):
    """优先队列实现"""
    def __init__(self):
        self.elements = []
        self.count = 0

    def empty(self) -> bool:
        """检查队列是否为空"""
        return len(self.elements) == 0

    def put(self, item: T, priority: float):
        """将元素加入队列"""
        heapq.heappush(self.elements, (priority, self.count, item))
        self.count += 1

    def get(self) -> T:
        """获取优先级最高的元素"""
        return heapq.heappop(self.elements)[2]

    def clear(self):
        """清空队列"""
        self.elements = []
        self.count = 0

class LazyThetaStarPathFinder(any_class):
    """Lazy Theta*路径规划算法，可与四叉树和八叉树协作"""

    def __init__(self, grid:Grid2D|Grid3D=None):
        self.grid = grid  # 添加grid参数支持
        self.open_set = PriorityQueue[PathNode]()
        self.all_nodes = {}
        self.closed_set = set()
        self.expanded_nodes = 0
        self.line_of_sight_checks = 0
        self.execution_time_ms = 0
        self.visited_nodes = []  # 用于可视化
        self.current_path = []   # 用于可视化

    def reset_stats(self):
        """重置统计数据"""
        self.expanded_nodes = 0
        self.line_of_sight_checks = 0
        self.execution_time_ms = 0
        self.visited_nodes = []
        self.current_path = []

    def heuristic[T:Vector3D|Vector2D](self, a:T , b: T) -> float:
        """启发式函数：欧几里得距离"""
        return a.distance_to(b)

    def get_node[T:Vector3D|Vector2D](self, position: T) -> PathNode:
        """获取或创建指定位置的节点"""
        if position not in self.all_nodes:
            self.all_nodes[position] = PathNode(position)
        return self.all_nodes[position]

    def reconstruct_path[T:Vector3D|Vector2D](self, goal_node: PathNode) -> List[T]:
        """重建从起点到终点的路径"""
        path = []
        current = goal_node

        while current:
            path.append(current.position)
            current = current.parent

        return list(reversed(path))

    def line_of_sight_quadtree[T:Vector3D|Vector2D](self, quadtree: QuadTree, start: T, end: T) -> bool:
        """使用四叉树检查两点之间是否有直线视野"""
        self.line_of_sight_checks += 1

        # 获取起点和终点的2D坐标
        x0, y0 = start.x, start.y
        x1, y1 = end.x, end.y

        # 计算直线参数
        dx = abs(x1 - x0)
        dy = abs(y1 - y0)
        sx = 1 if x0 < x1 else -1
        sy = 1 if y0 < y1 else -1
        err = dx - dy

        # 使用Bresenham算法检查直线上的点
        while x0 != x1 or y0 != y1:
            # 检查当前点是否在障碍物内
            current_point = (x0, y0)

            # 查询四叉树，检查当前点是否有障碍物
            range_rect = (x0-0.5, y0-0.5, 1, 1)  # 创建一个小范围来查询当前点
            points_in_range = quadtree.root.query(range_rect)

            if points_in_range:
                return False  # 有障碍物，没有直线视野

            # 移动到下一个点
            e2 = 2 * err
            if e2 > -dy:
                err -= dy
                x0 += sx
            if e2 < dx:
                err += dx
                y0 += sy

        # 检查终点
        range_rect = (x1-0.5, y1-0.5, 1, 1)
        points_in_range = quadtree.root.query(range_rect)

        return not points_in_range  # 如果终点没有障碍物，则有直线视野

    def line_of_sight_octree[T:Vector3D|Vector2D](self, octree: Octree, start: T, end: T) -> bool:
        """使用八叉树检查两点之间是否有直线视野"""
        self.line_of_sight_checks += 1

        # 获取起点和终点的3D坐标
        x0, y0, z0 = start.x, start.y, start.z
        x1, y1, z1 = end.x, end.y, end.z

        # 使用3D Bresenham算法
        dx = abs(x1 - x0)
        dy = abs(y1 - y0)
        dz = abs(z1 - z0)

        sx = 1 if x0 < x1 else -1
        sy = 1 if y0 < y1 else -1
        sz = 1 if z0 < z1 else -1

        # 确定最大步长方向
        if dx >= dy and dx >= dz:
            err1 = 2 * dy - dx
            err2 = 2 * dz - dx

            while x0 != x1:
                # 检查当前点是否在障碍物内
                current_point = (x0, y0, z0)

                # 查询八叉树，检查当前点是否有障碍物
                range_box = (x0-0.5, y0-0.5, z0-0.5, 1, 1, 1)
                points_in_range = octree.root.query(range_box)

                if points_in_range:
                    return False  # 有障碍物，没有直线视野

                # 移动到下一个点
                if err1 > 0:
                    y0 += sy
                    err1 -= 2 * dx

                if err2 > 0:
                    z0 += sz
                    err2 -= 2 * dx

                err1 += 2 * dy
                err2 += 2 * dz
                x0 += sx

        elif dy >= dx and dy >= dz:
            err1 = 2 * dx - dy
            err2 = 2 * dz - dy

            while y0 != y1:
                # 检查当前点
                current_point = (x0, y0, z0)
                range_box = (x0-0.5, y0-0.5, z0-0.5, 1, 1, 1)
                points_in_range = octree.root.query(range_box)

                if points_in_range:
                    return False

                if err1 > 0:
                    x0 += sx
                    err1 -= 2 * dy

                if err2 > 0:
                    z0 += sz
                    err2 -= 2 * dy

                err1 += 2 * dx
                err2 += 2 * dz
                y0 += sy

        else:
            err1 = 2 * dx - dz
            err2 = 2 * dy - dz

            while z0 != z1:
                # 检查当前点
                current_point = (x0, y0, z0)
                range_box = (x0-0.5, y0-0.5, z0-0.5, 1, 1, 1)
                points_in_range = octree.root.query(range_box)

                if points_in_range:
                    return False

                if err1 > 0:
                    x0 += sx
                    err1 -= 2 * dz

                if err2 > 0:
                    y0 += sy
                    err2 -= 2 * dz

                err1 += 2 * dx
                err2 += 2 * dy
                z0 += sz

        # 检查终点
        range_box = (x1-0.5, y1-0.5, z1-0.5, 1, 1, 1)
        points_in_range = octree.root.query(range_box)

        return not points_in_range

    def get_neighbors_quadtree[T:Vector3D|Vector2D](self, quadtree: QuadTree, position: T, neighbor_distance: float = 1.0) -> List[T]:
        """使用四叉树获取指定位置的相邻节点"""
        # 创建一个查询范围
        range_rect = (
            position.x - neighbor_distance,
            position.y - neighbor_distance,
            neighbor_distance * 2,
            neighbor_distance * 2
        )

        # 查询范围内的所有点
        all_points = quadtree.root.query(range_rect)

        # 过滤掉障碍物点，只保留可通行的点
        neighbors = []
        for point in all_points:
            # 创建Vector3D对象
            neighbor = Vector3D(point[0], point[1], 0)

            # 跳过当前位置
            if neighbor == position:
                continue

            # 添加到邻居列表
            neighbors.append(neighbor)

        return neighbors

    def get_neighbors_octree[T:Vector3D|Vector2D](self, octree: Octree, position: T, neighbor_distance: float = 1.0) -> List[T]:
        """使用八叉树获取指定位置的相邻节点"""
        # 创建一个查询范围
        range_box = (
            position.x - neighbor_distance,
            position.y - neighbor_distance,
            position.z - neighbor_distance,
            neighbor_distance * 2,
            neighbor_distance * 2,
            neighbor_distance * 2
        )

        # 查询范围内的所有点
        all_points = octree.root.query(range_box)

        # 过滤掉障碍物点，只保留可通行的点
        neighbors = []
        for point in all_points:
            # 创建Vector3D对象
            neighbor = Vector3D(point[0], point[1], point[2])

            # 跳过当前位置
            if neighbor == position:
                continue

            # 添加到邻居列表
            neighbors.append(neighbor)

        return neighbors

    def set_vertex[T:Vector3D|Vector2D](self, s: PathNode, tree: Union[QuadTree, Octree], is_octree: bool = False):
        """设置节点的父节点（Lazy Theta*的核心）"""
        if s.parent:
            # 检查节点是否可以"看到"其父节点
            has_line_of_sight = False

            if is_octree:
                has_line_of_sight = self.line_of_sight_octree(tree, s.parent.position, s.position)
            else:
                has_line_of_sight = self.line_of_sight_quadtree(tree, s.parent.position, s.position)

            if not has_line_of_sight:
                # 如果不能"看到"，找一个可见的邻居作为新的父节点
                min_g = float('inf')
                new_parent = None

                # 获取邻居节点
                neighbors = []
                if is_octree:
                    neighbors = self.get_neighbors_octree(tree, s.position)
                else:
                    neighbors = self.get_neighbors_quadtree(tree, s.position)

                for neighbor_pos in neighbors:
                    if neighbor_pos in self.closed_set:
                        neighbor = self.get_node(neighbor_pos)
                        tentative_g = neighbor.g + neighbor.position.distance_to(s.position)

                        if tentative_g < min_g:
                            min_g = tentative_g
                            new_parent = neighbor

                if new_parent:
                    s.parent = new_parent
                    s.g = min_g

    def find_path(self, start, goal):
        """为了兼容Visualization类的接口，使用grid进行路径规划"""
        import time
        self.reset_stats()
        start_time = time.time()

        # 初始化起点
        start_node = self.get_node(start)
        start_node.g = 0
        start_node.h = self.heuristic(start, goal)

        # 将起点加入开放列表
        self.open_set.put(start_node, start_node.f)

        while not self.open_set.empty():
            # 获取f值最小的节点
            current = self.open_set.get()
            self.visited_nodes.append(current.position)

            # 如果到达终点，重建路径并返回
            if current.position == goal:
                self.execution_time_ms = (time.time() - start_time) * 1000
                self.current_path = self.reconstruct_path(current)
                return self.current_path

            # 将当前节点加入闭集
            self.closed_set.add(current.position)
            self.expanded_nodes += 1

            # 检查所有相邻节点
            for neighbor_pos in self.grid.get_neighbors(current.position):
                # 如果相邻节点已在闭集中，跳过
                if neighbor_pos in self.closed_set:
                    continue

                neighbor = self.get_node(neighbor_pos)

                # Lazy Theta*的核心：假设相邻节点可以"看到"当前节点的父节点
                if current.parent:
                    # 检查是否有直线视野
                    self.line_of_sight_checks += 1
                    if self.grid.line_of_sight(current.parent.position, neighbor_pos):
                        # 如果有直线视野，计算从起点经过当前节点的父节点到相邻节点的代价
                        tentative_g = current.parent.g + current.parent.position.distance_to(neighbor_pos)
                    else:
                        # 如果没有直线视野，使用当前节点作为父节点
                        tentative_g = current.g + current.position.distance_to(neighbor_pos)
                        if tentative_g < neighbor.g:
                            neighbor.parent = current
                            neighbor.g = tentative_g
                            neighbor.h = self.heuristic(neighbor_pos, goal)
                            self.open_set.put(neighbor, neighbor.f)
                        continue
                else:
                    # 如果当前节点没有父节点（即起点），使用当前节点作为父节点
                    tentative_g = current.g + current.position.distance_to(neighbor_pos)

                if tentative_g < neighbor.g:
                    if current.parent and self.grid.line_of_sight(current.parent.position, neighbor_pos):
                        neighbor.parent = current.parent
                    else:
                        neighbor.parent = current

                    neighbor.g = tentative_g
                    neighbor.h = self.heuristic(neighbor_pos, goal)
                    self.open_set.put(neighbor, neighbor.f)

        # 如果开放列表为空且未找到路径，返回None
        self.execution_time_ms = (time.time() - start_time) * 1000
        return None

# 创建一个包装类，将LazyThetaStar与graph_core集成
class LazyThetaStarGraph(left_value_reference[GraphCore]):
    """将LazyThetaStar与graph_core集成的包装类"""

    def __init__(self, graph: GraphCore = None):
        """初始化LazyThetaStarGraph"""
        super().__init__(graph if graph is not None else GraphCore())
        self.pathfinder = LazyThetaStarPathFinder(self.ref_value)

    def create_quadtree_from_graph(self, boundary: Tuple[float, float, float, float], capacity: int = 4) -> QuadTree:
        """从图创建四叉树"""
        quadtree = self.ref_value.create_quadtree(boundary, capacity)

        # 将图中的节点添加到四叉树中
        for node in self.ref_value.get_nodes():
            # 假设节点属性中包含x和y坐标
            if 'x' in node and 'y' in node:
                quadtree.insert((node['x'], node['y']))

        return quadtree

    def create_octree_from_graph(self, boundary: Tuple[float, float, float, float, float, float], capacity: int = 8) -> Octree:
        """从图创建八叉树"""
        octree = self.ref_value.create_octree(boundary, capacity)

        # 将图中的节点添加到八叉树中
        for node in self.ref_value.get_nodes():
            # 假设节点属性中包含x、y和z坐标
            if 'x' in node and 'y' in node and 'z' in node:
                octree.insert((node['x'], node['y'], node['z']))

        return octree

    def find_path_2d(self, quadtree: QuadTree, start: Tuple[float, float], goal: Tuple[float, float], neighbor_distance: float = 1.0) -> List[Tuple[float, float]]:
        """使用四叉树寻找2D路径"""
        # 转换为Vector3D对象
        start_vec = Vector3D(start[0], start[1])
        goal_vec = Vector3D(goal[0], goal[1])

        # 寻找路径
        path = self.pathfinder.find_path(start_vec, goal_vec)

        # 转换回元组
        if path:
            return [(pos.x, pos.y) for pos in path]
        return None

    def find_path_3d(self, octree: Octree, start: Tuple[float, float, float], goal: Tuple[float, float, float], neighbor_distance: float = 1.0) -> List[Tuple[float, float, float]]:
        """使用八叉树寻找3D路径"""
        # 转换为Vector3D对象
        start_vec = Vector3D(start[0], start[1], start[2])
        goal_vec = Vector3D(goal[0], goal[1], goal[2])

        # 寻找路径
        path = self.pathfinder.find_path(start_vec, goal_vec)

        # 转换回元组
        if path:
            return [(pos.x, pos.y, pos.z) for pos in path]
        return None

    def get_stats(self) -> Dict[str, Any]:
        """获取路径规划的统计信息"""
        return {
            'expanded_nodes': self.pathfinder.expanded_nodes,
            'line_of_sight_checks': self.pathfinder.line_of_sight_checks,
            'execution_time_ms': self.pathfinder.execution_time_ms,
            'path_length': sum(self.pathfinder.current_path[i].distance_to(self.pathfinder.current_path[i+1])
                              for i in range(len(self.pathfinder.current_path)-1)) if self.pathfinder.current_path else 0
        }

def find_path_lazy_theta_star_2d(
    graph:                  GraphCore,
    start:                  Tuple[float, float],
    end:                    Tuple[float, float],
    boundary:               Tuple[float, float, float, float],
    capacity:               int = 4,
    neighbor_distance:      float = 1.0
    ) -> List[Tuple[float, float]]:
        """
        使用Lazy Theta*算法在2D空间中寻找路径

        Parameters
        ----------
        start : Tuple[float, float]
            起点坐标 (x, y)
        goal : Tuple[float, float]
            终点坐标 (x, y)
        boundary : Tuple[float, float, float, float]
            四叉树边界 (x, y, width, height)
        capacity : int, optional
            四叉树节点容量, by default 4
        neighbor_distance : float, optional
            邻居节点的距离阈值, by default 1.0

        Returns
        -------
        List[Tuple[float, float]]
            路径点列表，如果没有找到路径则返回None
        """
        lazy_theta_star = LazyThetaStarGraph(graph)

        # 创建四叉树
        quadtree = lazy_theta_star.create_quadtree_from_graph(boundary, capacity)

        # 寻找路径
        return lazy_theta_star.find_path_2d(quadtree, start, end, neighbor_distance)

def find_path_lazy_theta_star_3d(
    graph:                  GraphCore,
    start:                  Tuple[float, float, float],
    end:                    Tuple[float, float, float],
    boundary:               Tuple[float, float, float, float, float, float],
    capacity:               int = 8,
    neighbor_distance:      float = 1.0
    ) -> List[Tuple[float, float, float]]:
        """
        使用Lazy Theta*算法在3D空间中寻找路径

        Parameters
        ----------
        start : Tuple[float, float, float]
            起点坐标 (x, y, z)
        goal : Tuple[float, float, float]
            终点坐标 (x, y, z)
        boundary : Tuple[float, float, float, float, float, float]
            八叉树边界 (x, y, z, width, height, depth)
        capacity : int, optional
            八叉树节点容量, by default 8
        neighbor_distance : float, optional
            邻居节点的距离阈值, by default 1.0

        Returns
        -------
        List[Tuple[float, float, float]]
            路径点列表，如果没有找到路径则返回None
        """
        lazy_theta_star = LazyThetaStarGraph(graph)

        # 创建八叉树
        octree = lazy_theta_star.create_octree_from_graph(boundary, capacity)

        # 寻找路径
        return lazy_theta_star.find_path_3d(octree, start, end, neighbor_distance)

try:
    import pygame
    import pygame.font
    import time

    class Visualization:
        """Visualization class, responsible for drawing grids, paths and algorithm execution process"""

        # Color definition
        COLORS = {
            'background': (240, 240, 240),
            'grid_line': (200, 200, 200),
            'obstacle': (50, 50, 50),
            'start': (0, 200, 0),
            'goal': (200, 0, 0),
            'path': (0, 0, 200),
            'visited': (180, 180, 255),
            'text': (0, 0, 0),
            'button': (220, 220, 220),
            'button_hover': (200, 200, 200),
            'button_text': (0, 0, 0)
        }

        def __init__(self, width=800, height=600, grid_size=40, cell_size=15):
            """Initialize visualization"""
            pygame.init()
            pygame.font.init()

            self.width = width
            self.height = height
            self.grid_size = grid_size
            self.cell_size = cell_size
            self.grid_offset_x = 50
            self.grid_offset_y = 50

            self.screen = pygame.display.set_mode((width, height))
            pygame.display.set_caption("Lazy Theta* Algorithm Visualization")

            self.font = pygame.font.SysFont('Arial', 14)
            self.title_font = pygame.font.SysFont('Arial', 18, bold=True)

            # Create grid
            self.grid = Grid2D(grid_size, grid_size)

            # Create Lazy Theta* path planner
            self.lazy_theta_star = LazyThetaStarPathFinder(self.grid)

            # Start and end points
            self.start = Vector2D(5, 5)
            self.goal = Vector2D(grid_size - 5, grid_size - 5)

            # Status variables
            self.dragging = False
            self.erasing = False
            self.path_found = False
            self.running = True

            # Performance statistics
            self.stats = {
                'expanded_nodes': 0,
                'line_of_sight_checks': 0,
                'execution_time_ms': 0,
                'path_length': 0
            }

        def grid_to_screen(self, grid_x, grid_y):
            """Convert grid coordinates to screen coordinates"""
            screen_x = self.grid_offset_x + grid_x * self.cell_size
            screen_y = self.grid_offset_y + grid_y * self.cell_size
            return screen_x, screen_y

        def screen_to_grid(self, screen_x, screen_y):
            """Convert screen coordinates to grid coordinates"""
            grid_x = (screen_x - self.grid_offset_x) // self.cell_size
            grid_y = (screen_y - self.grid_offset_y) // self.cell_size

            if 0 <= grid_x < self.grid_size and 0 <= grid_y < self.grid_size:
                return grid_x, grid_y
            return None

        def draw_grid(self):
            """Draw grid"""
            # Draw background
            self.screen.fill(self.COLORS['background'])

            # Draw grid lines
            for i in range(self.grid_size + 1):
                # Vertical line
                start_pos = self.grid_to_screen(i, 0)
                end_pos = self.grid_to_screen(i, self.grid_size)
                pygame.draw.line(self.screen, self.COLORS['grid_line'], start_pos, end_pos)

                # Horizontal line
                start_pos = self.grid_to_screen(0, i)
                end_pos = self.grid_to_screen(self.grid_size, i)
                pygame.draw.line(self.screen, self.COLORS['grid_line'], start_pos, end_pos)

            # Draw obstacles
            for x in range(self.grid_size):
                for y in range(self.grid_size):
                    if self.grid.is_obstacle(x, y):
                        rect = pygame.Rect(
                            *self.grid_to_screen(x, y),
                            self.cell_size,
                            self.cell_size
                        )
                        pygame.draw.rect(self.screen, self.COLORS['obstacle'], rect)

        def draw_visited_nodes(self):
            """Draw visited nodes"""
            for pos in self.lazy_theta_star.visited_nodes:
                rect = pygame.Rect(
                    *self.grid_to_screen(pos.x, pos.y),
                    self.cell_size,
                    self.cell_size
                )
                pygame.draw.rect(self.screen, self.COLORS['visited'], rect)

        def draw_path(self):
            """Draw path"""
            if not self.lazy_theta_star.current_path:
                return

            # Calculate path length
            path_length = 0
            for i in range(1, len(self.lazy_theta_star.current_path)):
                prev = self.lazy_theta_star.current_path[i-1]
                curr = self.lazy_theta_star.current_path[i]
                path_length += prev.distance_to(curr)

            self.stats['path_length'] = path_length

            # Draw path line
            for i in range(1, len(self.lazy_theta_star.current_path)):
                prev = self.lazy_theta_star.current_path[i-1]
                curr = self.lazy_theta_star.current_path[i]

                start_pos = self.grid_to_screen(prev.x, prev.y)
                start_pos = (start_pos[0] + self.cell_size // 2, start_pos[1] + self.cell_size // 2)

                end_pos = self.grid_to_screen(curr.x, curr.y)
                end_pos = (end_pos[0] + self.cell_size // 2, end_pos[1] + self.cell_size // 2)

                pygame.draw.line(self.screen, self.COLORS['path'], start_pos, end_pos, 2)

        def draw_start_goal(self):
            """Draw start and end points"""
            # Draw start point
            rect = pygame.Rect(
                *self.grid_to_screen(self.start.x, self.start.y),
                self.cell_size,
                self.cell_size
            )
            pygame.draw.rect(self.screen, self.COLORS['start'], rect)

            # Draw end point
            rect = pygame.Rect(
                *self.grid_to_screen(self.goal.x, self.goal.y),
                self.cell_size,
                self.cell_size
            )
            pygame.draw.rect(self.screen, self.COLORS['goal'], rect)

        def draw_stats(self):
            """Draw performance statistics"""
            # Update statistics
            self.stats['expanded_nodes'] = self.lazy_theta_star.expanded_nodes
            self.stats['line_of_sight_checks'] = self.lazy_theta_star.line_of_sight_checks
            self.stats['execution_time_ms'] = self.lazy_theta_star.execution_time_ms

            # Draw title
            title = self.title_font.render("Algorithm: Lazy Theta*", True, self.COLORS['text'])
            self.screen.blit(title, (self.width - 300, 50))

            # Draw statistics
            y_offset = 80
            for key, value in self.stats.items():
                if key == 'path_length':
                    text = self.font.render(f"Path Length: {value:.2f}", True, self.COLORS['text'])
                elif key == 'execution_time_ms':
                    text = self.font.render(f"Execution Time: {value:.2f} ms", True, self.COLORS['text'])
                elif key == 'expanded_nodes':
                    text = self.font.render(f"Expanded Nodes: {value}", True, self.COLORS['text'])
                elif key == 'line_of_sight_checks':
                    text = self.font.render(f"Line of Sight Checks: {value}", True, self.COLORS['text'])

                self.screen.blit(text, (self.width - 300, y_offset))
                y_offset += 25

            # Draw operation instructions
            y_offset = self.height - 150
            instructions = [
                "Left click: Set start point",
                "Right click: Set end point",
                "Middle click drag: Create obstacle",
                "Space key: Start/Reset path planning",
                "C key: Clear all obstacles",
                "ESC key: Exit program"
            ]

            for instruction in instructions:
                text = self.font.render(instruction, True, self.COLORS['text'])
                self.screen.blit(text, (self.width - 300, y_offset))
                y_offset += 25

        def find_path(self):
            """Execute path planning"""
            # Reset algorithm state
            self.lazy_theta_star.open_set.clear()
            self.lazy_theta_star.all_nodes = {}
            self.lazy_theta_star.closed_set.clear()

            # Execute path planning
            path = self.lazy_theta_star.find_path(self.start, self.goal)

            if path:
                self.path_found = True
            else:
                self.path_found = False

        def handle_events(self):
            """Handle user input events"""
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False

                elif event.type == pygame.KEYDOWN:
                    # Space key: Start/Reset path planning
                    if event.key == pygame.K_SPACE:
                        self.find_path()

                    # ESC key: Exit program
                    elif event.key == pygame.K_ESCAPE:
                        self.running = False

                    # C key: Clear all obstacles
                    elif event.key == pygame.K_c:
                        self.grid.clear()
                        self.path_found = False

                elif event.type == pygame.MOUSEBUTTONDOWN:
                    grid_pos = self.screen_to_grid(*event.pos)

                    if grid_pos:
                        x, y = grid_pos

                        # Left click: Set start point
                        if event.button == 1:
                            self.start = Vector2D(x, y)
                            self.path_found = False

                        # Right click: Set end point
                        elif event.button == 3:
                            self.goal = Vector2D(x, y)
                            self.path_found = False

                        # Middle click: Start drawing/erasing obstacles
                        elif event.button == 2:
                            self.dragging = True
                            self.erasing = self.grid.is_obstacle(x, y)
                            self.grid.set_obstacle(x, y, not self.erasing)
                            self.path_found = False

                elif event.type == pygame.MOUSEBUTTONUP:
                    # Stop drawing/erasing obstacles
                    if event.button == 2:
                        self.dragging = False

                elif event.type == pygame.MOUSEMOTION:
                    # Draw/erase obstacles
                    if self.dragging:
                        grid_pos = self.screen_to_grid(*event.pos)

                        if grid_pos:
                            x, y = grid_pos
                            self.grid.set_obstacle(x, y, not self.erasing)
                            self.path_found = False

        def run(self):
            """Run the visualization program"""
            clock = pygame.time.Clock()

            while self.running:
                self.handle_events()

                # Draw
                self.draw_grid()

                if self.path_found:
                    self.draw_visited_nodes()
                    self.draw_path()

                self.draw_start_goal()
                self.draw_stats()

                pygame.display.flip()
                clock.tick(60)

            pygame.quit()

except ImportError:
    pass
