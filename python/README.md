# Convention

Convention是一个综合性的Python工具库，提供了多个实用模块：

## 功能模块

- [Algorithm](./Convention/Algorithm/README.md): 算法
- [Audio](./Convention/Audio/README.md): 音频
- [DataBase](./Convention/DataBase/README.md): 数据
- [File](./Convention/File/README.md): 文件操作
- [Lang](./Convention/Lang/README.md): 与python语言本身相关的内容
- [LLM](./Convention/LLM/README.md): 大语言模型
- [MathEx](./Convention/MathEx/README.md): 数学扩展
- [ML](./Convention/ML/README.md): 机器学习
- [Str](./Convention/Str/README.md): 字符串处理与转换
- [UI](./Convention/UI/README.md): 界面与控件
- [Visual](./Convention/Visual/README.md): 可视化
- [Web](./Convention/Web/README.md): Web工具

## 安装与构建

使用python构建:

```bash
python -m build
```

使用pip安装：

```bash
pip install dist/Convention-version.tar.gz
```

或者

```bash
pip install dist/Convention-version.whl
```

或者获取来自github的release版本

```bash
pip install https://github.com/NINEMINEsigma/Convention/releases/download/v0.3.2-python-part/convention-0.3.2.tar.gz
```

## 生成依赖文件

```python
import Convention
ReleaseFailed2Requirements()
```

## 使用示例

使用tool_file的统一接口将资源加载

```python
from Convention.Str.Core import *
from Convention.File.Core import *

image = tool_file("image.png").load() # PIL.ImageFile.ImageFile
txt = tool_file("text.json").load_as_txt() # str
json = tool_file("text.json").load() # json

```

当依赖的包缺失时会自动提示如何使用pip安装缺失的依赖
你也可以使用

```python
ReleaseFailed2Requirements()
```

生成一份requirements.txt文件, 随后使用

```bash
pip install -r requirements.txt
```

安装缺失的所有内容

## 许可证

本项目采用MIT许可证。详见 [LICENSE](LICENSE) 文件
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
