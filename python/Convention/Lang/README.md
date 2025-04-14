# Lang 模块

Lang模块提供了Python语言相关的扩展功能，包括反射、序列化、并发编程等特性。

## 目录结构

- `Core.py`: 核心语言功能
- `Reflection.py`: 反射功能
- `EasySave.py`: 序列化功能
- `CppLike.py`: C++风格的功能封装

## 反射模块 (Reflection.py)

反射模块提供了强大的类型系统和运行时类型操作功能。它允许在运行时检查、创建和操作类型、对象、方法和属性。

### 核心功能

#### 1. 类型系统

- **类型转换与解析**
  - 支持从字符串、对象、类型等多种形式转换为类型
  - 内置类型缓存机制，提高性能
  - 支持泛型类型解析
  - 支持模块级别的类型解析

- **类型信息管理**
  - 类型元数据管理
  - 类型继承关系处理
  - 类型成员访问控制
  - 类型验证和检查

#### 2. 反射操作

- **成员访问**
  - 字段访问和修改
  - 方法调用
  - 属性操作
  - 静态成员处理

- **类型创建**
  - 实例创建
  - 类型动态创建
  - 泛型类型实例化

### 技术细节

#### 1. 性能优化

- **多级缓存机制**
  ```python
  _type_string_cache: Dict[str, type] = {}  # 类型字符串缓存
  _weak_refs: Dict[int, "weakref.ref[RefType]"] = {}  # 弱引用缓存
  _type_name_cache: Dict[str, type] = {}  # 类型名称缓存
  _string_to_type_cache: Dict[str, Any] = {}  # 字符串到类型缓存
  ```

- **延迟初始化**
  ```python
  def _ensure_initialized(self):
      """确保完全初始化，实现延迟加载"""
      if self._initialized:
          return
      # 初始化逻辑...
  ```

#### 2. 类型系统实现

- **类型标志系统**
  ```python
  class RefTypeFlag(IntFlag):
      Static = 0b00000001
      Instance = 0b00000010
      Public = 0b00000100
      Private = 0b00001000
      Default = 0b00010000
      Method = 0b00100000
      Field = 0b01000000
      Special = 0b10000000
      All = 0b11111111
  ```

- **泛型支持**
  ```python
  type RTypen[_T] = RefType
  ```

#### 3. 线程安全

- **锁机制**
  ```python
  _RefTypeLock:Dict[Any|type|_SpecialIndictaor, threading.Lock] = {}
  ```

### 使用示例

#### 1. 基本类型操作

```python
from Convention.Lang import TypeManager

# 获取类型管理器实例
type_manager = TypeManager.GetInstance()

# 创建或获取类型的反射信息
ref_type = type_manager.CreateOrGetRefType(MyClass)

# 获取类型的所有字段
fields = ref_type.GetFields()
for field in fields:
    print(f"Field: {field.FieldName}, Type: {field.FieldType}")
```

#### 2. 成员访问

```python
# 获取字段值
value = ref_type.GetFieldValue(obj, "field_name")

# 设置字段值
ref_type.SetFieldValue(obj, "field_name", new_value)

# 调用方法
result = ref_type.InvokeMethod(obj, "method_name", arg1, arg2)
```

#### 3. 类型检查

```python
# 验证类型
is_valid = ref_type.Verify(value_type)

# 获取类型树
type_tree = ref_type.tree()
```

### 高级特性

1. **类型树生成**
   - 支持生成类型的完整继承关系树
   - 包含所有字段和方法信息
   - 支持JSON格式输出

2. **泛型支持**
   - 支持泛型类型解析
   - 支持泛型参数处理
   - 支持泛型约束

3. **元数据管理**
   - 支持类型注解
   - 支持属性访问控制
   - 支持静态成员处理

### 性能考虑

1. **缓存策略**
   - 使用多级缓存减少重复计算
   - 使用弱引用避免内存泄漏
   - 延迟初始化减少启动开销

2. **线程安全**
   - 使用锁机制保护共享资源
   - 支持并发访问
   - 避免死锁

3. **内存管理**
   - 自动垃圾回收
   - 弱引用支持
   - 资源及时释放

### 注意事项

1. 使用反射功能时注意类型安全
2. 并发编程时注意线程安全
3. 大量使用反射可能影响性能
4. 注意内存使用和垃圾回收

## 其他功能特性

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

### 2. 序列化功能 (EasySave.py)

#### 2.1 序列化设置

- 文件格式选择
- 备份选项
- 字段过滤
- 编码设置

#### 2.2 序列化操作

- JSON序列化
- 二进制序列化
- 对象保存
- 对象加载

### 3. C++风格功能 (CppLike.py)

#### 3.1 容器操作

- 元组创建
- 列表创建
- 字典创建
- 映射创建

#### 3.2 算法操作

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

### 2. 序列化示例

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

### 3. C++风格示例

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

