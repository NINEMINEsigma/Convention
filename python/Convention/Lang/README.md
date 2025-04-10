# Lang 模块

Lang模块提供了Python语言相关的扩展功能，包括反射、序列化、并发编程等特性。

## 目录结构

- `Core.py`: 核心语言功能
- `Reflection.py`: 反射功能
- `EasySave.py`: 序列化功能
- `CppLike.py`: C++风格的功能封装

## 功能特性

### 1. 核心语言功能 (Core.py)

#### 1.1 并发编程

- 线程管理
  - 线程创建和控制
  - 线程同步
  - 线程池
- 进程管理
  - 进程创建和控制
  - 进程间通信
  - 共享内存

#### 1.2 工具函数

- 元素分割
- 异步协程支持
- 调试工具

### 2. 反射功能 (Reflection.py)

#### 2.1 类型系统

- 类型信息获取
- 类型转换
- 泛型支持
- 类型检查

#### 2.2 反射操作

- 类成员访问
- 方法调用
- 属性操作
- 实例创建

#### 2.3 元数据管理

- 类型管理器
- 成员信息
- 方法信息
- 参数信息

### 3. 序列化功能 (EasySave.py)

#### 3.1 序列化设置

- 文件格式选择
- 备份选项
- 字段过滤
- 编码设置

#### 3.2 序列化操作

- JSON序列化
- 二进制序列化
- 对象保存
- 对象加载

### 4. C++风格功能 (CppLike.py)

#### 4.1 容器操作

- 元组创建
- 列表创建
- 字典创建
- 映射创建

#### 4.2 算法操作

- 最大值查找
- 最小值查找
- 边界值计算

## 使用示例

### 1. 并发编程示例

```python
from Convention.Lang import ThreadCore

# 创建线程
def worker():
    print("Working...")

thread = ThreadCore(worker)
thread.start()
thread.join()
```

### 2. 反射功能示例

```python
from Convention.Lang import TypeManager

# 获取类型信息
type_manager = TypeManager.GetInstance()
ref_type = type_manager.CreateOrGetRefType(MyClass)

# 获取方法信息
methods = ref_type.GetMethods()
for method in methods:
    print(method.MethodName)
```

### 3. 序列化示例

```python
from Convention.Lang import EasySave

# 保存对象
class MyClass:
    def __init__(self):
        self.value = 42

obj = MyClass()
EasySave.Write(obj, "data.json")

# 加载对象
loaded_obj = EasySave.Read(MyClass, "data.json")
```

### 4. C++风格示例

```python
from Convention.Lang import CppLike

# 创建容器
my_list = CppLike.make_list(1, 2, 3)
my_dict = CppLike.make_dict(a=1, b=2)

# 使用算法
max_index = CppLike.max_element([1, 3, 2, 5, 4])
```

## 依赖项

- pydantic: 数据验证
- typing: 类型提示
- multiprocessing: 进程管理
- threading: 线程管理
- json: JSON处理

## 注意事项

1. 使用反射功能时注意类型安全
2. 并发编程时注意线程安全
3. 序列化时注意数据兼容性
4. 使用共享内存时注意同步

## 性能优化

1. 使用线程池管理线程
2. 合理使用进程间通信
3. 序列化时选择合适的格式
4. 反射操作时缓存类型信息

## 贡献指南

欢迎提交Issue和Pull Request来改进功能或添加新特性。
