# File 模块

File模块提供了强大的文件操作功能，包括文件读写、压缩加密、备份监控等。

## 目录结构

- `Core.py`: 核心文件操作功能
- `__init__.py`: 模块初始化文件

## 功能特性

### 1. 基础文件操作

#### 1.1 文件读写

- 支持多种文件格式
  - 文本文件 (txt, md)
  - 数据文件 (json, csv, xml)
  - 办公文档 (xlsx, xls, docx, doc)
  - 图像文件 (png, jpg, jpeg, bmp, svg, ico)
  - 音频文件 (mp3, ogg, wav)
- 自动识别文件类型
- 支持二进制和文本模式

#### 1.2 文件管理

- 文件创建、删除、复制、移动
- 目录遍历和操作
- 文件属性获取
- 路径处理

### 2. 高级功能

#### 2.1 文件压缩

- 支持ZIP格式
- 支持TAR格式
- 压缩和解压缩

#### 2.2 文件加密

- AES加密
- 支持自定义加密算法
- 加密和解密

#### 2.3 文件监控

- 实时监控文件变化
- 支持递归监控
- 可配置忽略模式
- 事件回调

#### 2.4 文件备份

- 自动备份
- 版本控制
- 备份恢复
- 元数据管理

#### 2.5 权限管理

- 读写执行权限
- 隐藏属性
- 递归权限设置

## 使用示例

### 1. 基础文件操作

```python
from Convention.File import tool_file

# 创建文件对象
file = tool_file("example.txt")

# 写入内容
file.write("Hello, World!")

# 读取内容
content = file.load_as_text()
print(content)

# 复制文件
file.copy("example_copy.txt")
```

### 2. 数据文件操作

```python
# 读取JSON文件
json_data = tool_file("data.json").load_as_json()

# 读取CSV文件
csv_data = tool_file("data.csv").load_as_csv()

# 读取Excel文件
excel_data = tool_file("data.xlsx").load_as_excel()
```

### 3. 高级功能使用

```python
# 文件压缩
file.compress("archive.zip")

# 文件加密
file.encrypt("secret_key", algorithm="AES")

# 文件监控
def on_file_changed(event_type, path):
    print(f"File {path} was {event_type}")

file.start_monitoring(on_file_changed)

# 创建备份
file.create_backup(backup_dir="backups", max_backups=5)
```

## 依赖项

- pandas: 数据处理
- pydub: 音频处理
- Pillow: 图像处理
- python-docx: Word文档处理
- watchdog: 文件监控

## 注意事项

1. 文件操作时注意异常处理
2. 加密操作需要妥善保管密钥
3. 监控功能会占用系统资源
4. 备份功能需要足够的存储空间

## 性能优化

1. 大文件操作时使用流式处理
2. 监控功能可以设置忽略模式减少资源占用
3. 压缩时选择合适的压缩级别

## 贡献指南

欢迎提交Issue和Pull Request来改进功能或添加新特性。
