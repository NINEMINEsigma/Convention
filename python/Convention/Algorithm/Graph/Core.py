from ...Internal import *
from ...MathEx.Core import *
from ...Visual.OpenCV import ImageObject, Wrapper as Wrapper2ImageObject
import cv2
import numpy as np
import networkx as base
from networkx.classes.reportviews import NodeView, EdgeView

type graph_node_type = Any
type data_type = dict[str, Any]
type weight_label_type = str|Callable[[graph_node_type, graph_node_type, data_type], Optional[NumberLike]]

class QuadTreeNode(any_class):
    """四叉树节点"""
    def __init__(self, boundary, capacity=4):
        """
        初始化四叉树节点

        Parameters
        ----------
        boundary : tuple
            边界 (x, y, width, height)
        capacity : int
            节点容量
        """
        self.boundary = boundary  # 边界 (x, y, width, height)
        self.capacity = capacity  # 节点容量
        self.points = []  # 存储点
        self.divided = False  # 是否已分割
        self.northwest = None  # 西北象限
        self.northeast = None  # 东北象限
        self.southwest = None  # 西南象限
        self.southeast = None  # 东南象限

    def subdivide(self):
        """将当前节点分割为四个子节点"""
        x, y, w, h = self.boundary
        nw = (x, y, w / 2, h / 2)  # 西北象限
        ne = (x + w / 2, y, w / 2, h / 2)  # 东北象限
        sw = (x, y + h / 2, w / 2, h / 2)  # 西南象限
        se = (x + w / 2, y + h / 2, w / 2, h / 2)  # 东南象限
        self.northwest = QuadTreeNode(nw, self.capacity)
        self.northeast = QuadTreeNode(ne, self.capacity)
        self.southwest = QuadTreeNode(sw, self.capacity)
        self.southeast = QuadTreeNode(se, self.capacity)
        self.divided = True

    def contains(self, point):
        """检查点是否在边界内"""
        x, y, w, h = self.boundary
        px, py = point
        return (x <= px < x + w) and (y <= py < y + h)

    def insert(self, point):
        """插入一个点"""
        if not self.contains(point):
            return False

        if len(self.points) < self.capacity:
            self.points.append(point)
            return True
        else:
            if not self.divided:
                self.subdivide()

            return (self.northwest.insert(point) or
                    self.northeast.insert(point) or
                    self.southwest.insert(point) or
                    self.southeast.insert(point))

    def query(self, range_rect):
        """查询范围内的点"""
        found_points = []
        if not self._intersects(range_rect):
            return found_points

        # 检查当前节点中的点
        for point in self.points:
            if self._point_in_range(point, range_rect):
                found_points.append(point)

        # 如果已分割，则递归查询子节点
        if self.divided:
            found_points.extend(self.northwest.query(range_rect))
            found_points.extend(self.northeast.query(range_rect))
            found_points.extend(self.southwest.query(range_rect))
            found_points.extend(self.southeast.query(range_rect))

        return found_points

    def _intersects(self, range_rect):
        """检查范围是否与当前边界相交"""
        rx, ry, rw, rh = range_rect
        x, y, w, h = self.boundary
        return not (rx + rw < x or rx > x + w or ry + rh < y or ry > y + h)

    def _point_in_range(self, point, range_rect):
        """检查点是否在范围内"""
        rx, ry, rw, rh = range_rect
        px, py = point
        return (rx <= px < rx + rw) and (ry <= py < ry + rh)

class OctreeNode(any_class):
    """八叉树节点"""
    def __init__(self, boundary, capacity=8):
        """
        初始化八叉树节点

        Parameters
        ----------
        boundary : tuple
            边界 (x, y, z, width, height, depth)
        capacity : int
            节点容量
        """
        self.boundary = boundary  # 边界 (x, y, z, width, height, depth)
        self.capacity = capacity  # 节点容量
        self.points = []  # 存储点
        self.divided = False  # 是否已分割

        # 八个子节点
        self.front_northwest = None
        self.front_northeast = None
        self.front_southwest = None
        self.front_southeast = None
        self.back_northwest = None
        self.back_northeast = None
        self.back_southwest = None
        self.back_southeast = None

    def subdivide(self):
        """将当前节点分割为八个子节点"""
        x, y, z, w, h, d = self.boundary

        # 前四个象限
        nw_front = (x, y, z, w / 2, h / 2, d / 2)  # 西北前象限
        ne_front = (x + w / 2, y, z, w / 2, h / 2, d / 2)  # 东北前象限
        sw_front = (x, y + h / 2, z, w / 2, h / 2, d / 2)  # 西南前象限
        se_front = (x + w / 2, y + h / 2, z, w / 2, h / 2, d / 2)  # 东南前象限

        # 后四个象限
        nw_back = (x, y, z + d / 2, w / 2, h / 2, d / 2)  # 西北后象限
        ne_back = (x + w / 2, y, z + d / 2, w / 2, h / 2, d / 2)  # 东北后象限
        sw_back = (x, y + h / 2, z + d / 2, w / 2, h / 2, d / 2)  # 西南后象限
        se_back = (x + w / 2, y + h / 2, z + d / 2, w / 2, h / 2, d / 2)  # 东南后象限

        self.front_northwest = OctreeNode(nw_front, self.capacity)
        self.front_northeast = OctreeNode(ne_front, self.capacity)
        self.front_southwest = OctreeNode(sw_front, self.capacity)
        self.front_southeast = OctreeNode(se_front, self.capacity)
        self.back_northwest = OctreeNode(nw_back, self.capacity)
        self.back_northeast = OctreeNode(ne_back, self.capacity)
        self.back_southwest = OctreeNode(sw_back, self.capacity)
        self.back_southeast = OctreeNode(se_back, self.capacity)

        self.divided = True

    def contains(self, point):
        """检查点是否在边界内"""
        x, y, z, w, h, d = self.boundary
        px, py, pz = point
        return (x <= px < x + w) and (y <= py < y + h) and (z <= pz < z + d)

    def insert(self, point):
        """插入一个点"""
        if not self.contains(point):
            return False

        if len(self.points) < self.capacity:
            self.points.append(point)
            return True
        else:
            if not self.divided:
                self.subdivide()

            return (self.front_northwest.insert(point) or
                    self.front_northeast.insert(point) or
                    self.front_southwest.insert(point) or
                    self.front_southeast.insert(point) or
                    self.back_northwest.insert(point) or
                    self.back_northeast.insert(point) or
                    self.back_southwest.insert(point) or
                    self.back_southeast.insert(point))

    def query(self, range_box):
        """查询范围内的点"""
        found_points = []
        if not self._intersects(range_box):
            return found_points

        # 检查当前节点中的点
        for point in self.points:
            if self._point_in_range(point, range_box):
                found_points.append(point)

        # 如果已分割，则递归查询子节点
        if self.divided:
            found_points.extend(self.front_northwest.query(range_box))
            found_points.extend(self.front_northeast.query(range_box))
            found_points.extend(self.front_southwest.query(range_box))
            found_points.extend(self.front_southeast.query(range_box))
            found_points.extend(self.back_northwest.query(range_box))
            found_points.extend(self.back_northeast.query(range_box))
            found_points.extend(self.back_southwest.query(range_box))
            found_points.extend(self.back_southeast.query(range_box))

        return found_points

    def _intersects(self, range_box):
        """检查范围是否与当前边界相交"""
        rx, ry, rz, rw, rh, rd = range_box
        x, y, z, w, h, d = self.boundary
        return not (rx + rw < x or rx > x + w or
                   ry + rh < y or ry > y + h or
                   rz + rd < z or rz > z + d)

    def _point_in_range(self, point, range_box):
        """检查点是否在范围内"""
        rx, ry, rz, rw, rh, rd = range_box
        px, py, pz = point
        return ((rx <= px < rx + rw) and
                (ry <= py < ry + rh) and
                (rz <= pz < rz + rd))

class QuadTree(any_class):
    """四叉树"""
    def __init__(self, boundary, capacity=4):
        """
        初始化四叉树

        Parameters
        ----------
        boundary : tuple
            边界 (x, y, width, height)
        capacity : int
            节点容量
        """
        self.root = QuadTreeNode(boundary, capacity)

    def insert(self, point):
        """插入一个点"""
        return self.root.insert(point)

    def query(self, range_rect):
        """查询范围内的点"""
        return self.root.query(range_rect)

class Octree(any_class):
    """八叉树"""
    def __init__(self, boundary, capacity=8):
        """
        初始化八叉树

        Parameters
        ----------
        boundary : tuple
            边界 (x, y, z, width, height, depth)
        capacity : int
            节点容量
        """
        self.root = OctreeNode(boundary, capacity)

    def insert(self, point):
        """插入一个点"""
        return self.root.insert(point)

    def query(self, range_box):
        """查询范围内的点"""
        return self.root.query(range_box)

class graph_core(left_value_reference[base.Graph]):
    def __init__(self, graph:Optional[base.Graph] = None, is_directed:bool = False):
        if graph is None and is_directed:
            graph = base.DiGraph()
        elif graph is None:
            graph = base.Graph()
        super().__init__(graph)

    def is_directed(self) -> bool:
        """判断图是否是有向图"""
        return self.ref_value.is_directed()

    def is_acyclic(self) -> bool:
        """判断图是否是无环图"""
        return base.is_directed_acyclic_graph(self.ref_value)

    def is_dag(self) -> bool:
        """判断图是否是有向无环图"""
        return base.is_directed_acyclic_graph(self.ref_value)

    def is_weakly_connected(self) -> bool:
        """判断图是否是弱连通图"""
        return base.is_weakly_connected(self.ref_value)

    def build_graph(self, nodes:list[graph_node_type], edges:list[tuple[graph_node_type, graph_node_type, data_type]]) -> Self:
        """构建图"""
        self.ref_value = base.Graph()
        self.ref_value.add_nodes_from(nodes)
        self.ref_value.add_edges_from(edges)
        return self

    def add_node(self, node:graph_node_type, **attr) -> Self:
        """添加节点标签与节点带有的属性"""
        self.ref_value.add_node(node, **attr)
        return self

    def add_edge(self, u:graph_node_type, v:graph_node_type, **attr) -> Self:
        """添加边标签与边带有的属性"""
        self.ref_value.add_edge(u, v, **attr)
        return self

    def remove_node(self, node:graph_node_type) -> Self:
        """删除节点"""
        self.ref_value.remove_node(node)
        return self

    def remove_edge(self, u:graph_node_type, v:graph_node_type):
        """删除边"""
        self.ref_value.remove_edge(u, v)
        return self

    def get_neighbors(self, node:graph_node_type) -> list[graph_node_type]:
        """获取节点的邻居"""
        return list(self.ref_value.neighbors(node))

    def get_degree(self, node:graph_node_type) -> int:
        """获取节点的度"""
        return self.ref_value.degree[node]

    def get_out_degree(self, node:graph_node_type) -> int:
        """获取节点的出度"""
        return self.ref_value.out_degree[node]

    def get_in_degree(self, node:graph_node_type) -> int:
        """获取节点的入度"""
        return self.ref_value.in_degree[node]

    def get_nodes(self) -> NodeView:
        """获取所有节点, 返回值可视为list[data_type]"""
        return self.ref_value.nodes

    def get_edges(self) -> EdgeView:
        """获取所有边, 返回值可视为list[tuple[graph_node_type, graph_node_type, data_type]]"""
        return self.ref_value.edges

    def contains_node(self, node:graph_node_type) -> bool:
        """判断是否存在节点"""
        return self.ref_value.has_node(node)

    def contains_edge(self, u:graph_node_type, v:graph_node_type) -> bool:
        """判断是否存在边"""
        return self.ref_value.has_edge(u, v)

    def dijkstra(
        self,
        start_node:             graph_node_type,
        cutoff_length:          Optional[NumberLike]    = None,
        weight_or_edgeCounter:  str|Callable[[graph_node_type, graph_node_type, data_type], Optional[NumberLike]]
                                                        = "weight"
        ) -> (
        list[Optional[graph_node_type]] |
        dict[graph_node_type, list[graph_node_type]]
        ):
        """使用 Dijkstra 算法计算从起始节点到所有其他节点的最短路径"""
        return base.single_source_dijkstra_path(self.ref_value, start_node, cutoff=cutoff_length, weight=weight_or_edgeCounter)

    def dijkstra_length(
        self,
        start_node:             graph_node_type,
        cutoff_length:          Optional[NumberLike]    = None,
        weight_or_edgeCounter:  weight_label_type       = "weight"
        ) -> (
        tuple[Literal[0], list[graph_node_type | None]] |
        tuple[dict, dict[graph_node_type, list[graph_node_type]]] |
        tuple[graph_node_type, list[graph_node_type]]
        ):
        """计算从起始节点到所有其他节点的最短路径长度"""
        return base.single_source_dijkstra(self.ref_value, start_node, cutoff=cutoff_length, weight=weight_or_edgeCounter)

    def kruskal(self) -> base.Graph:
        """使用 Kruskal 算法计算最小生成树, 树的类型与图的类型相同"""
        return base.minimum_spanning_tree(self.ref_value)

    def dfs(self, start_node:graph_node_type) -> list[graph_node_type]:
        """深度优先搜索, 搜索结果为可达节点列表"""
        visited = set()
        result = []

        def dfs_helper(node):
            if node not in visited:
                visited.add(node)
                result.append(node)
                for neighbor in self.ref_value.neighbors(node):
                    dfs_helper(neighbor)

        dfs_helper(start_node)
        return result

    def bfs(self, start_node:graph_node_type) -> list[graph_node_type]:
        """广度优先搜索, 搜索结果为可达节点列表"""
        visited = set()
        queue = [start_node]
        result = []

        while queue:
            node = queue.pop(0)
            if node not in visited:
                visited.add(node)
                result.append(node)
                queue.extend(neighbor for neighbor in self.ref_value.neighbors(node) if neighbor not in visited)

        return result

    def connected_components(self) -> Generator[set[graph_node_type], Any, None]:
        """查找图中的连通分量, 返回值为生成器, 生成器中的每个元素为连通分量中的节点列表"""
        return base.connected_components(self.ref_value)

    def is_connected(self) -> bool:
        """判断图是否连通"""
        return base.is_connected(self.ref_value)

    def connected_component(self, node:graph_node_type) -> set[graph_node_type]:
        """查找节点所在的连通分量"""
        return base.node_connected_component(self.ref_value, node)

    def strongly_connected_components(self) -> Generator[set[graph_node_type], Any, None]:
        """查找图中的强连通分量, 返回值为生成器, 生成器中的每个元素为强连通分量中的节点列表"""
        return base.strongly_connected_components(self.ref_value)

    def condensation(self) -> tuple[base.Graph, dict[graph_node_type, set[graph_node_type]]]:
        """将图中的强连通分量收缩为一个点, 返回新图和每个点对应的原图中的节点集合

        Returns
        -------
        Graph
            收缩后的新图
        dict[graph_node_type, set[graph_node_type]]
            每个点对应的原图中的节点集合
        """
        components:     list[set[graph_node_type]] = list(self.strongly_connected_components())
        component_map:  dict[graph_node_type, int] = {}
        for i, component in enumerate(components):
            for node in component:
                component_map[node] = i

        # 创建与原图相同类型的图
        C = self.ref_value.__class__()
        C.add_nodes_from(range(len(components)))

        # 通过component_map将原图的边添加到新图中
        for u, v in self.ref_value.edges():
            ci = component_map[u]
            cj = component_map[v]
            # 如果两个节点不在同一个强连通分量中, 则添加边
            # 否则将会属于同一个节点
            if ci != cj:
                C.add_edge(ci, cj)

        # 保存每个点对应的原图中的节点集合
        mapping = {i: component for i, component in enumerate(components)}
        base.set_node_attributes(C, mapping, 'nodes')

        return C, mapping

    def topological_sort(self) -> Generator[graph_node_type, Any, None]:
        """对有向无环图进行拓扑排序, 返回值为生成器, 生成器中的每个元素为拓扑排序后的节点"""
        return base.topological_sort(self.ref_value)

    def bellman_ford(
        self,
        start_node:             graph_node_type,
        weight_or_edgeCounter:  weight_label_type       = "weight"
        ) -> (
        tuple[Literal[0], list[graph_node_type | None]] |
        tuple[dict, dict[graph_node_type, list[graph_node_type]]] |
        tuple[graph_node_type, list[graph_node_type]]
        ):
        """使用 Bellman-Ford 算法计算从起始节点到所有其他节点的最短路径"""
        return base.single_source_bellman_ford_path(self.ref_value, start_node, weight=weight_or_edgeCounter)

    def floyd_warshall(self) -> (
        tuple[dict[graph_node_type, dict[graph_node_type, NumberLike]], dict[graph_node_type, dict[graph_node_type, graph_node_type]]] |
        tuple[dict[graph_node_type, dict[graph_node_type, NumberLike]], dict[graph_node_type, dict[graph_node_type, graph_node_type]]]
        ):
        """使用 Floyd-Warshall 算法计算所有节点对之间的最短路径"""
        return base.floyd_warshall(self.ref_value)

    def johnson(self) -> (
        tuple[dict[graph_node_type, dict[graph_node_type, NumberLike]], dict[graph_node_type, dict[graph_node_type, graph_node_type]]] |
        tuple[dict[graph_node_type, dict[graph_node_type, NumberLike]], dict[graph_node_type, dict[graph_node_type, graph_node_type]]]
        ):
        """使用 Johnson 算法计算所有节点对之间的最短路径"""
        return base.johnson(self.ref_value)

    def minimum_spanning_tree(
        self,
        algorithm:                  str,
        weight_or_edgeCounter:      weight_label_type   = "weight",
        ignore_nan:                 bool                = False
        ) -> base.Graph:
        """使用最小生成树算法计算最小生成树, 树的类型与图的类型相同"""
        return base.minimum_spanning_tree(self.ref_value, weight=weight_or_edgeCounter, algorithm=algorithm, ignore_nan=ignore_nan)

    def get_minimum_spanning_tree_algorithms(self) -> list[str]:
        """获取适用于minimum_spanning_tree函数的算法列表"""
        return ["kruskal", "prim", "boruvka", "random", "greedy", "reverse_delete"]

    def prim(
        self,
        weight_or_edgeCounter:      weight_label_type   = "weight",
        ignore_nan:                 bool                = False
        ) -> base.Graph:
        """使用 Prim 算法计算最小生成树, 树的类型与图的类型相同"""
        return self.minimum_spanning_tree(self.ref_value, weight=weight_or_edgeCounter, algorithm="prim", ignore_nan=ignore_nan)

    def kruskal(
        self,
        weight_or_edgeCounter:      weight_label_type   = "weight",
        ignore_nan:                 bool                = False
        ) -> base.Graph:
        """使用 Kruskal 算法计算最小生成树, 树的类型与图的类型相同"""
        return self.minimum_spanning_tree(self.ref_value, weight=weight_or_edgeCounter, algorithm="kruskal", ignore_nan=ignore_nan)

    def boruvka(
        self,
        weight_or_edgeCounter:      weight_label_type   = "weight",
        ignore_nan:                 bool                = False
        ) -> base.Graph:
        """使用 Boruvka 算法计算最小生成树, 树的类型与图的类型相同"""
        return self.minimum_spanning_tree(self.ref_value, weight=weight_or_edgeCounter, algorithm="boruvka", ignore_nan=ignore_nan)

    def minimum_spanning_edges(
        self,
        weight_or_edgeCounter:      weight_label_type   = "weight",
        ignore_nan:                 bool                = False
        ) -> list[tuple[graph_node_type, graph_node_type, data_type]]:
        """使用最小生成树算法计算最小生成树的边, 返回值为列表, 列表中的每个元素为最小生成树的边"""
        return base.minimum_spanning_edges(self.ref_value, weight=weight_or_edgeCounter, ignore_nan=ignore_nan)

    def create_quadtree(self, boundary, capacity=4) -> QuadTree:
        """
        创建四叉树

        Parameters
        ----------
        boundary : tuple
            边界 (x, y, width, height)
        capacity : int
            节点容量

        Returns
        -------
        QuadTree
            四叉树
        """
        return QuadTree(boundary, capacity)

    def create_octree(self, boundary, capacity=8) -> Octree:
        """
        创建八叉树

        Parameters
        ----------
        boundary : tuple
            边界 (x, y, z, width, height, depth)
        capacity : int
            节点容量

        Returns
        -------
        Octree
            八叉树
        """
        return Octree(boundary, capacity)

def QuadTree2Graph(tree:QuadTree) -> graph_core:
        """将四叉树转换为图"""
        G = graph_core()

        # 使用BFS遍历四叉树并构建图
        queue = [tree.root]
        while queue:
            node = queue.pop(0)

            # 添加当前节点
            node_id = id(node)
            G.add_node(node_id, boundary=node.boundary, points=node.points)

            # 如果节点已分割，添加子节点和边
            if node.divided:
                children = [node.northwest, node.northeast, node.southwest, node.southeast]
                for child in children:
                    if child:
                        child_id = id(child)
                        G.add_node(child_id, boundary=child.boundary, points=child.points)
                        G.add_edge(node_id, child_id)
                        queue.append(child)

        return G

def Graph3QuadTree(G: graph_core) -> QuadTree:
        """从图构建四叉树

        Parameters
        ----------
        G : graph_core
            图, 节点属性中必须包含boundary和points

        Returns
        -------
        QuadTree
            四叉树
        """
        tree:QuadTree = QuadTree()
        # 找到根节点(入度为0的节点)
        root_id = None
        for node in G.get_nodes():
            if G.get_in_degree(node) == 0:
                root_id = node
                break

        if root_id is None:
            raise ValueError("图中没有根节点")

        # 获取根节点的属性
        root_attrs = G.get_nodes()[root_id]
        tree.root = QuadTreeNode(root_attrs["boundary"], capacity=4)
        tree.root.points = root_attrs["points"]

        # 使用BFS重建四叉树
        queue = [(root_id, tree.root)]
        while queue:
            parent_id, parent_node = queue.pop(0)

            # 获取子节点
            children = []
            for neighbor in G.get_neighbors(parent_id):
                if G.get_in_degree(neighbor) > 0:  # 确保是父节点指向子节点
                    children.append(neighbor)

            # 如果有子节点，说明已分割
            if children:
                parent_node.divided = True
                # 根据boundary位置确定象限
                for child_id in children:
                    child_attrs = G.get_nodes()[child_id]
                    child_boundary = child_attrs["boundary"]
                    child_node = QuadTreeNode(child_boundary, capacity=4)
                    child_node.points = child_attrs["points"]

                    # 根据边界位置判断是哪个象限
                    px, py = child_boundary[0], child_boundary[1]
                    parent_x, parent_y = parent_node.boundary[0], parent_node.boundary[1]

                    if px == parent_x:
                        if py == parent_y:
                            parent_node.northwest = child_node
                        else:
                            parent_node.southwest = child_node
                    else:
                        if py == parent_y:
                            parent_node.northeast = child_node
                        else:
                            parent_node.southeast = child_node

                    queue.append((child_id, child_node))

        return tree

def Octree2Graph(tree:Octree) -> graph_core:
        """将八叉树转换为图"""
        G = graph_core()

        # 使用BFS遍历八叉树并构建图
        queue = [tree.root]
        while queue:
            node = queue.pop(0)

            # 添加当前节点
            node_id = id(node)
            G.add_node(node_id, boundary=node.boundary, points=node.points)

            # 如果节点已分割，添加子节点和边
            if node.divided:
                children = [
                    node.front_northwest, node.front_northeast,
                    node.front_southwest, node.front_southeast,
                    node.back_northwest, node.back_northeast,
                    node.back_southwest, node.back_southeast
                ]
                for child in children:
                    if child:
                        child_id = id(child)
                        G.add_node(child_id, boundary=child.boundary, points=child.points)
                        G.add_edge(node_id, child_id)
                        queue.append(child)

        return G

def Graph2Octree(G: graph_core) -> Octree:
        """从图构建八叉树

        Parameters
        ----------
        G : graph_core
            图对象,需要包含节点属性boundary和points

        Returns
        -------
        Octree
            八叉树对象
        """
        tree:Octree = Octree()
        # 获取根节点
        root_node = None
        for node in G.get_nodes():
            if len(G.get_in_degree(node)) == 0:
                root_node = node
                break

        if root_node is None:
            raise ValueError("输入图中没有根节点")

        # 获取根节点的属性
        boundary = G.get_nodes()[root_node]["boundary"]
        points = G.get_nodes()[root_node]["points"]

        # 创建新的八叉树
        tree.root = OctreeNode(boundary, capacity=8)
        tree.root.points = points.copy()

        # 使用BFS重建树结构
        queue = [(root_node, tree.root)]
        while queue:
            graph_node, tree_node = queue.pop(0)

            # 获取子节点
            children = G.get_neighbors(graph_node)
            if children:
                tree_node.divided = True
                for child in children:
                    child_boundary = G.get_nodes()[child]["boundary"]
                    child_points = G.get_nodes()[child]["points"]

                    # 根据边界位置确定子节点类型
                    x, y, z = child_boundary[:3]
                    px, py, pz = tree_node.boundary[:3]

                    child_node = OctreeNode(child_boundary, tree_node.capacity)
                    child_node.points = child_points.copy()

                    # 根据相对位置设置对应的子节点
                    if z < pz + tree_node.boundary[5]/2:  # front
                        if x < px + tree_node.boundary[3]/2:  # west
                            if y < py + tree_node.boundary[4]/2:  # northwest
                                tree_node.front_northwest = child_node
                            else:  # southwest
                                tree_node.front_southwest = child_node
                        else:  # east
                            if y < py + tree_node.boundary[4]/2:  # northeast
                                tree_node.front_northeast = child_node
                            else:  # southeast
                                tree_node.front_southeast = child_node
                    else:  # back
                        if x < px + tree_node.boundary[3]/2:  # west
                            if y < py + tree_node.boundary[4]/2:  # northwest
                                tree_node.back_northwest = child_node
                            else:  # southwest
                                tree_node.back_southwest = child_node
                        else:  # east
                            if y < py + tree_node.boundary[4]/2:  # northeast
                                tree_node.back_northeast = child_node
                            else:  # southeast
                                tree_node.back_southeast = child_node

                    queue.append((child, child_node))

        return tree

def load_quadtree_from_bw_image(img:ImageObject, capacity:int = 4) -> QuadTree:
    """
    从黑白图像中加载四叉树，每个黑色像素点都是一个节点，白色像素点则不包含节点

    Parameters
    ----------
    image : ImageObject
        图像对象
    capacity : int, optional
        四叉树节点容量, by default 4

    Returns
    -------
    QuadTree
        加载的四叉树
    """
    # 导入必要的模块

    # 加载图像并转换为灰度图
    if img.channels > 1:
        img = img.get_grayscale()

    # 确保图像是二值化的（黑白图像）
    _, binary_img = cv2.threshold(img.image, 127, 255, cv2.THRESH_BINARY)

    # 创建四叉树，边界为图像大小
    height, width = binary_img.shape
    quadtree = QuadTree((0, 0, width, height), capacity)

    # 遍历图像中的每个像素
    for y in range(height):
        for x in range(width):
            # 如果像素是黑色（值为0），则将其添加到四叉树中
            if binary_img[y, x] == 0:
                quadtree.insert((x, y))

    return quadtree

def visualize_quadtree(quadtree:QuadTree, output_path:str, width:int|None = None, height:int|None = None) -> ImageObject:
    """
    将四叉树可视化为图像

    Parameters
    ----------
    quadtree : QuadTree
        要可视化的四叉树
    output_path : str
        输出图像路径
    width : int, optional
        输出图像宽度，默认使用四叉树边界宽度
    height : int, optional
        输出图像高度，默认使用四叉树边界高度
    """

    # 获取四叉树边界
    x, y, w, h = quadtree.root.boundary

    # 如果未指定宽度和高度，则使用四叉树边界
    if width is None:
        width = int(w)
    if height is None:
        height = int(h)

    # 创建空白图像（白色背景）
    img = np.ones((height, width), dtype=np.uint8) * 255

    # 递归绘制四叉树节点
    def draw_node(node:QuadTreeNode):
        # 绘制当前节点中的点
        for point in node.points:
            px, py = point
            if 0 <= px < width and 0 <= py < height:
                img[int(py), int(px)] = 0  # 黑色

        # 如果节点已分割，则递归绘制子节点
        if node.divided:
            draw_node(node.northwest)
            draw_node(node.northeast)
            draw_node(node.southwest)
            draw_node(node.southeast)

    # 绘制四叉树
    draw_node(quadtree.root)

    # 保存图像
    cv2.imwrite(output_path, img)

    return Wrapper2ImageObject(img)

def load_quadtree_from_bw_image_with_subdivision(img:ImageObject, min_size:int = 1, threshold:int = 127) -> QuadTree:
    """
    从黑白图像中加载四叉树，使用递归细分方法

    Parameters
    ----------
    img : ImageObject
        图像对象
    min_size : int, optional
        最小细分尺寸，当区域小于此尺寸时停止细分，默认为1
    threshold : int, optional
        二值化阈值，默认为127

    Returns
    -------
    QuadTree
        加载的四叉树
    """
    # 加载图像并转换为灰度图
    if img.channels > 1:
        img = img.get_grayscale()

    # 确保图像是二值化的（黑白图像）
    _, binary_img = cv2.threshold(img.image, threshold, 255, cv2.THRESH_BINARY)

    # 创建四叉树，边界为图像大小
    height, width = binary_img.shape
    quadtree = QuadTree((0, 0, width, height), capacity=1)  # 容量设为1，因为我们使用细分方法

    # 递归细分图像
    def subdivide_image(node:QuadTreeNode, x:int, y:int, w:int, h:int):
        # 检查当前区域是否全为白色
        region = binary_img[int(y):int(y+h), int(x):int(x+w)]
        if np.all(region == 255):
            # 全白区域，不需要细分
            return

        # 检查当前区域是否全为黑色
        if np.all(region == 0):
            # 全黑区域，添加中心点
            center_x = x + w/2
            center_y = y + h/2
            node.points.append((center_x, center_y))
            return

        # 如果区域太小，不再细分
        if w <= min_size or h <= min_size:
            # 添加区域中的黑色像素点
            for py in range(int(y), int(y+h)):
                for px in range(int(x), int(x+w)):
                    if binary_img[py, px] == 0:
                        node.points.append((px, py))
            return

        # 细分当前节点
        node.subdivide()

        # 递归处理四个子区域
        half_w = w / 2
        half_h = h / 2
        subdivide_image(node.northwest, x, y, half_w, half_h)
        subdivide_image(node.northeast, x + half_w, y, half_w, half_h)
        subdivide_image(node.southwest, x, y + half_h, half_w, half_h)
        subdivide_image(node.southeast, x + half_w, y + half_h, half_w, half_h)

    # 开始递归细分
    subdivide_image(quadtree.root, 0, 0, width, height)

    return quadtree




