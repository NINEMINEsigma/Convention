# Str 模块

Str模块提供了字符串处理相关的功能，包括字符串操作、正则表达式处理和SVG转换等。

## 目录结构

- `Core.py`: 核心字符串处理功能
- `RE.py`: 正则表达式处理功能

## 功能特性

### 1. 字符串操作 (Core.py)

#### 1.1 基础操作

- 字符串限制
  - 长度限制
  - 填充处理
- 字符串连接
  - 符号连接
  - 列表连接
- 字节转换
  - 字节列表转字符串列表
  - 字节列表转字符串

#### 1.2 字符串类

- 属性操作
  - 内容获取/设置
  - 长度获取
- 修改操作
  - 追加
  - 清空
  - 插入
  - 删除
  - 替换
- 查找操作
  - 查找子串
  - 截取子串
- 转换操作
  - 大小写转换
  - 空白处理
  - 字符修剪
- 判断操作
  - 前缀判断
  - 后缀判断
  - 包含判断
  - 空值判断

#### 1.3 SVG转换

- SVG生成
  - 文本转SVG
  - 文件保存
- 配置选项
  - 尺寸设置
  - 字体设置
  - 颜色设置
  - 对齐方式
  - 多行处理
  - 阴影效果

#### 1.4 GIF转换

- 标签GIF生成
  - 文本转GIF
  - 文件保存
- 动画效果
  - 帧率控制
  - 过渡效果
  - 颜色渐变

### 2. 正则表达式 (RE.py)

#### 2.1 正则类

- 模式匹配
  - 起始匹配
  - 搜索匹配
  - 全部匹配
  - 替换匹配
- 编译优化
  - 预编译模式
  - 重复使用

#### 2.2 模式构建

- 基础模式
  - 数字匹配
  - 任意字符
  - 字符组
  - 非数字
- 空白处理
  - 空白字符
  - 非空白字符
- 单词处理
  - 单词字符
  - 非单词字符
- 重复模式
  - 零次或多次
  - 一次或多次
  - 零次或一次
  - 指定次数
  - 次数范围
- 位置匹配
  - 起始位置
  - 结束位置
- 组合操作
  - 原始字符
  - 或操作

## 使用示例

### 1. 字符串操作示例

```python
from Convention.Str import light_str

# 创建字符串对象
s = light_str("Hello World")

# 基本操作
s.append("!")
s.insert(5, ",")
s.replace("World", "Python")

# 转换操作
upper_s = s.upper()
stripped_s = s.strip()

# 判断操作
if s.startswith("Hello"):
    print("Starts with Hello")
```

### 2. SVG转换示例

```python
# 文本转SVG
svg_text = s.to_svg(
    width=300,
    height=100,
    font_size=24,
    font_family="Arial",
    text_color="black",
    bg_color="white"
)

# 保存为文件
s.to_svg(output_file="output.svg")
```

### 3. 正则表达式示例

```python
from Convention.Str import light_re, number, follow_exist

# 创建正则表达式
regex = light_re(number() + follow_exist())

# 匹配操作
test_string = "123 abc 456"
match_result = regex.match(test_string)
search_result = regex.search(test_string)
findall_result = regex.find_all(test_string)

# 替换操作
new_string = regex.sub("<number>", test_string)
```

## 依赖项

- re: 正则表达式支持
- xml.etree.ElementTree: XML处理
- xml.dom.minidom: XML美化
- pathlib: 路径处理

## 注意事项

1. 字符串操作注意编码问题
2. SVG生成注意尺寸设置
3. 正则表达式注意性能优化
4. 文件操作注意路径处理

## 性能优化

1. 使用预编译的正则表达式
2. 合理设置字符串缓冲区
3. 避免频繁的字符串拼接
4. 使用适当的字符串方法

## 贡献指南

欢迎提交Issue和Pull Request来改进功能或添加新特性。
