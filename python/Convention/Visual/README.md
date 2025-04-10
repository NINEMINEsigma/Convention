# Visual 模块

Visual模块提供了数据可视化和图像处理相关的功能，包括数据图表、图像处理、词云等。

## 目录结构

- `Core.py`: 核心数据可视化功能
- `OpenCV.py`: OpenCV图像处理功能
- `WordCloud.py`: 词云生成功能
- `Manim.py`: 数学动画功能

## 功能特性

### 1. 数据可视化 (Core.py)

#### 1.1 基础图表

- 折线图
- 柱状图
- 散点图
- 直方图
- 饼图
- 箱线图
- 热力图
- 分类数据图
- 联合图

#### 1.2 数据处理

- 缺失值处理
- 重复值处理
- 数据标准化
- 数据归一化

### 2. 图像处理 (OpenCV.py)

#### 2.1 图像操作

- 图像加载
  - 支持多种格式（jpg, png, bmp等）
  - 支持从文件路径或URL加载
  - 支持从内存缓冲区加载
- 图像保存
  - 支持多种格式输出
  - 支持质量参数设置
  - 支持压缩选项
- 图像显示
  - 支持窗口标题设置
  - 支持窗口大小调整
  - 支持键盘事件处理
- 图像转换
  - RGB转灰度
  - RGB转HSV
  - RGB转LAB
  - 支持自定义转换矩阵
- 图像缩放
  - 支持多种插值方法
  - 支持保持宽高比
  - 支持指定目标尺寸
- 图像旋转
  - 支持任意角度旋转
  - 支持旋转中心点设置
  - 支持旋转后尺寸调整
- 图像翻转
  - 水平翻转
  - 垂直翻转
  - 对角线翻转
- 图像合并
  - 支持多图像拼接
  - 支持透明度混合
  - 支持蒙版处理

#### 2.2 ImageObject类详解

ImageObject类提供了完整的图像处理功能：

```python
from Convention.Visual import OpenCV

# 创建图像对象
image = OpenCV.ImageObject("input.jpg")

# 基本属性
width = image.width          # 图像宽度
height = image.height        # 图像高度
channels = image.channels    # 通道数
dtype = image.dtype         # 数据类型

# 图像处理
image.resize_image(800, 600)                    # 调整大小
image.convert_to_grayscale()                    # 转换为灰度图
image.filter_gaussian((5, 5), 1.5, 1.5)         # 高斯滤波
image.rotate_image(45)                          # 旋转45度
image.flip_image(horizontal=True)               # 水平翻转

# 图像增强
image.adjust_brightness(1.2)                    # 调整亮度
image.adjust_contrast(1.5)                      # 调整对比度
image.adjust_saturation(0.8)                    # 调整饱和度
image.equalize_histogram()                      # 直方图均衡化

# 边缘检测
image.detect_edges(threshold1=100, threshold2=200)  # Canny边缘检测
image.detect_contours()                          # 轮廓检测

# 特征提取
keypoints = image.detect_keypoints()            # 关键点检测
descriptors = image.compute_descriptors()       # 描述子计算

# 图像保存
image.save_image("output.jpg", quality=95)      # 保存图像
image.save_image("output.png", compression=9)   # 保存PNG

# 图像显示
image.show_image("预览")                        # 显示图像
image.wait_key(0)                               # 等待按键

# 图像信息
print(image.get_info())                         # 获取图像信息
print(image.get_histogram())                    # 获取直方图
```

#### 2.3 图像增强

- 边缘检测
- 滤波处理
- 阈值处理
- 形态学操作
- 轮廓检测
- 特征匹配

#### 2.4 视频处理

- 视频读取
- 视频写入
- 摄像头控制
- 帧处理

### 3. 词云生成 (WordCloud.py)

#### 3.1 词云功能

- 词云创建
- 标题设置
- 渲染输出
- 样式定制

### 4. 数学动画 (Manim.py)

#### 4.1 动画功能

- 数学公式动画
- 几何图形动画
- 图表动画
- 场景管理

## 使用示例

### 1. 数据可视化示例

```python
from Convention.Visual import Core

# 创建数据可视化生成器
generator = Core.data_visual_generator("data.csv")

# 绘制折线图
generator.plot_line("x", "y", title="折线图示例")

# 绘制柱状图
generator.plot_bar("category", "value", title="柱状图示例")

# 绘制散点图
generator.plot_scatter("x", "y", title="散点图示例")

# 绘制饼图
generator.plot_pie("category", title="饼图示例")
```

### 2. 图像处理示例

```python
from Convention.Visual import OpenCV

# 创建图像对象
image = OpenCV.ImageObject("input.jpg")

# 图像处理
image.resize_image(800, 600)
image.convert_to_grayscale()
image.filter_gaussian((5, 5), 1.5, 1.5)

# 保存图像
image.save_image("output.jpg")
```

### 3. 词云生成示例

```python
from Convention.Visual import WordCloud

# 创建词云
wordcloud = WordCloud.make_word_cloud("词云", [
    ("Python", 100),
    ("Java", 80),
    ("C++", 70),
    ("JavaScript", 90),
])

# 设置标题
WordCloud.set_title(wordcloud, "编程语言词云")

# 渲染输出
WordCloud.render_to(wordcloud, "wordcloud.html")
```

### 4. 视频处理示例

```python
from Convention.Visual import OpenCV

# 创建视频捕获对象
camera = OpenCV.light_cv_camera(0)

# 创建视频写入对象
writer = OpenCV.VideoWriterInstance(
    "output.avi",
    OpenCV.avi_with_Xvid_fourcc(),
    30.0,
    (640, 480)
)

# 录制视频
def stop_condition():
    return OpenCV.is_current_key('q')

camera.recording(stop_condition, writer)
```

## 依赖项

- matplotlib: 数据可视化
- seaborn: 高级数据可视化
- opencv-python: 图像处理
- pyecharts: 词云生成
- manim: 数学动画

## 注意事项

1. 使用图像处理时注意内存占用
2. 视频处理时注意帧率设置
3. 词云生成时注意数据量
4. 动画制作时注意性能优化

## 性能优化

1. 使用图像处理时注意批量处理
2. 视频处理时使用合适的编码格式
3. 词云生成时控制词数
4. 动画制作时优化渲染设置

## 贡献指南

欢迎提交Issue和Pull Request来改进功能或添加新特性。
