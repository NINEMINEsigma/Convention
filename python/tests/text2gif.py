
import os

# 将Convention文件夹加入到系统路径
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from Convention.Str.Core import *
from Convention.File.Core import tool_file

ReleaseFailed2Requirements()

# 测试代码
text = "/LiuBai/"
converter = label_gif_converter()
converter.to_label_gif(text, "output.gif",
    font_size=360, # 增大字体以适应更大的分辨率
    font_family="Arial Black", # 使用粗体字体
    text_color="black",
    bg_color="white",
    duration=50,
)
