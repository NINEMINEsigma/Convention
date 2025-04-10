# MathEx 模块

MathEx模块提供了数学相关的扩展功能，包括数值计算、几何变换、方程求解等。

## 目录结构

- `Core.py`: 核心数学功能
- `Transform.py`: 几何变换功能

## 功能特性

### 1. 核心数学功能 (Core.py)

#### 1.1 数值计算

- 数值类型支持
  - 基本数值类型
  - NumPy数组
  - 数值引用
- 数值操作
  - 算术运算
  - 位运算
  - 比较运算
- 数值处理
  - 标准化
  - 规范化
  - 裁剪

#### 1.2 方程求解

- 方程组求解
- 优化求解
- 数值积分

### 2. 几何变换 (Transform.py)

#### 2.1 矩形操作

- 矩形定义
- 矩形变换
  - 移动
  - 缩放
  - 旋转
- 矩形运算
  - 交集
  - 并集
  - 碰撞检测

#### 2.2 四元数

- 四元数定义
- 四元数运算
  - 加法
  - 减法
  - 乘法
  - 共轭
  - 逆
- 四元数变换

## 使用示例

### 1. 数值计算示例

```python
from Convention.MathEx import Core

# 创建数值引用
num_ref = Core.left_number_reference(42)

# 数值运算
result = num_ref + 10
print(result)  # 52

# 数组操作
arr_ref = Core.left_np_ndarray_reference(np.array([1, 2, 3]))
result = arr_ref * 2
print(result)  # [2, 4, 6]
```

### 2. 方程求解示例

```python
from Convention.MathEx import Core

# 定义方程
def equation(x):
    return x**2 - 4

# 求解方程
solver = Core.solver()
result = solver.opt_fsolve(equation, 1)
print(result)  # 2.0
```

### 3. 矩形操作示例

```python
from Convention.MathEx import Transform

# 创建矩形
box = Transform.Box(0, 10, 10, 0)

# 移动矩形
box.move(5, 5)

# 缩放矩形
box.scale(2, 2)

# 检查碰撞
other_box = Transform.Box(5, 15, 15, 5)
is_collide = box.is_collide(other_box)
print(is_collide)  # True
```

### 4. 四元数示例

```python
from Convention.MathEx import Transform

# 创建四元数
q1 = Transform.Quaternion(1, 2, 3, 4)
q2 = Transform.Quaternion(5, 6, 7, 8)

# 四元数运算
result = q1 * q2
print(result)  # (-60, 12, 30, 24)

# 四元数变换
conjugate = q1.conjugate()
inverse = q1.inverse()
```

## 依赖项

- numpy: 数值计算
- scipy: 科学计算
- cv2: 图像处理

## 注意事项

1. 使用数值引用时注意内存管理
2. 方程求解时注意收敛性
3. 几何变换时注意坐标系
4. 四元数运算时注意单位

## 性能优化

1. 使用NumPy数组加速计算
2. 合理使用数值引用
3. 优化方程求解参数
4. 使用向量化操作

## 贡献指南

欢迎提交Issue和Pull Request来改进功能或添加新特性。
