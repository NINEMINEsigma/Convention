from pathlib import Path
from ..Internal                 import *
from ..Lang.Reflection          import light_reflection
import                                 re
from pathlib                    import Path
import xml.etree.ElementTree    as     ET
from xml.dom                    import minidom
import                                 math

def limit_str(data, max_length:int=50):
    s:str = data if data is str else str(data)
    if len(s) <= max_length:
        return s
    else:
        inside_str = "\n...\n...\n"
        # 计算头尾部分的长度
        head_length = max_length // 2
        tail_length = max_length - head_length - len(inside_str)  # 3 是省略号的长度

        # 截取头尾部分并连接
        return s[:head_length] + inside_str + s[-tail_length:]

def fill_str(data, max_length:int=50, fill_char:str=" ", side:Literal["left", "right", "center"]="right"):
    s:str = data if data is str else str(data)
    char = fill_char[0]
    if len(s) >= max_length:
        return s
    else:
        if side == "left":
            return s + char * (max_length - len(s))
        elif side == "right":
            return char * (max_length - len(s)) + s
        elif side == "center":
            left = (max_length - len(s)) // 2
            right = max_length - len(s) - left
            return char * left + s + char * right
        else:
            raise ValueError(f"Unsupported side: {side}")

def link(symbol:str, strs:list):
    return symbol.join(strs)

def list_byte_to_list_string(lines:List[bytes], encoding='utf-8') -> List[str]:
    return [line.decode(encoding) for line in lines]

def list_byte_to_string(lines:List[bytes], encoding='utf-8') -> str:
    return "".join(list_byte_to_list_string(lines, encoding))

class str2svg_converter(any_class):
    def __init__(self):
        self.namespace = "http://www.w3.org/2000/svg"
        ET.register_namespace('', self.namespace)

    def create_svg(self, config:dict) -> ET.Element:
        """创建SVG根元素"""
        svg = ET.Element('svg')
        svg.set('xmlns', self.namespace)
        svg.set('width', str(config['width']))
        svg.set('height', str(config['height']))
        svg.set('viewBox', f"0 0 {config['width']} {config['height']}")

        # 添加背景
        if config.get('bg_color'):
            bg = ET.SubElement(svg, 'rect')
            bg.set('width', '100%')
            bg.set('height', '100%')
            bg.set('fill', config['bg_color'])

        return svg

    def add_text(self, parent:ET.Element, text:str, config:dict) -> ET.Element:
        """添加文本元素"""
        text_elem = ET.SubElement(parent, 'text')

        # 设置位置
        if config['align'] == 'center':
            text_elem.set('x', '50%')
            text_elem.set('text-anchor', 'middle')
        elif config['align'] == 'right':
            text_elem.set('x', str(config['width'] - config['padding']))
            text_elem.set('text-anchor', 'end')
        else:  # left
            text_elem.set('x', str(config['padding']))
            text_elem.set('text-anchor', 'start')

        # 垂直对齐
        if config['valign'] == 'middle':
            text_elem.set('y', '50%')
            text_elem.set('dominant-baseline', 'middle')
        elif config['valign'] == 'bottom':
            text_elem.set('y', str(config['height'] - config['padding']))
            text_elem.set('dominant-baseline', 'text-after-edge')
        else:  # top
            text_elem.set('y', str(config['padding'] + config['font_size']))
            text_elem.set('dominant-baseline', 'text-before-edge')

        # 样式设置
        text_elem.set('font-family', config['font_family'])
        text_elem.set('font-size', str(config['font_size']))
        text_elem.set('fill', config['text_color'])

        if config.get('font_weight'):
            text_elem.set('font-weight', config['font_weight'])

        if config.get('font_style'):
            text_elem.set('font-style', config['font_style'])

        if config.get('letter_spacing'):
            text_elem.set('letter-spacing', str(config['letter_spacing']))

        # 添加文本效果
        if config.get('text_shadow'):
            defs = self._ensure_defs(parent)
            filter_id = 'shadow_filter'
            self._add_shadow_filter(defs, filter_id, config['text_shadow'])
            text_elem.set('filter', f'url(#{filter_id})')

        # 处理多行文本
        if config.get('multiline') and '\n' in text:
            lines = text.split('\n')
            line_height = config['font_size'] * 1.2  # 默认行高

            if config.get('line_height'):
                line_height = config['line_height']

            # 清除文本内容，将使用tspan代替
            text_elem.text = None

            for i, line in enumerate(lines):
                tspan = ET.SubElement(text_elem, 'tspan')
                tspan.set('x', text_elem.get('x'))

                # 计算每行y位置
                if config['valign'] == 'middle':
                    # 调整中间对齐的垂直位置
                    total_height = line_height * (len(lines) - 1)
                    base_y = config['height'] / 2 - total_height / 2
                    y_pos = base_y + i * line_height
                elif config['valign'] == 'bottom':
                    base_y = config['height'] - config['padding'] - line_height * (len(lines) - 1)
                    y_pos = base_y + i * line_height
                else:  # top
                    y_pos = config['padding'] + config['font_size'] + i * line_height

                tspan.set('y', str(y_pos))
                tspan.text = line
        else:
            text_elem.text = text

        return text_elem

    def _ensure_defs(self, svg:ET.Element) -> ET.Element:
        """确保SVG有defs元素，如果没有则创建"""
        defs = svg.find('.//{%s}defs' % self.namespace)
        if defs is None:
            defs = ET.SubElement(svg, 'defs')
        return defs

    def _add_shadow_filter(self, defs:ET.Element, filter_id:str, shadow_config:dict) -> None:
        """添加阴影滤镜"""
        # 解析阴影配置
        offset_x = shadow_config.get('offset_x', 2)
        offset_y = shadow_config.get('offset_y', 2)
        blur = shadow_config.get('blur', 2)
        color = shadow_config.get('color', 'rgba(0,0,0,0.5)')

        # 创建滤镜
        filter_elem = ET.SubElement(defs, 'filter')
        filter_elem.set('id', filter_id)

        # 创建阴影效果
        fe_offset = ET.SubElement(filter_elem, 'feOffset')
        fe_offset.set('in', 'SourceAlpha')
        fe_offset.set('dx', str(offset_x))
        fe_offset.set('dy', str(offset_y))
        fe_offset.set('result', 'offOut')

        # 模糊
        fe_gaussian = ET.SubElement(filter_elem, 'feGaussianBlur')
        fe_gaussian.set('in', 'offOut')
        fe_gaussian.set('stdDeviation', str(blur))
        fe_gaussian.set('result', 'blurOut')

        # 颜色
        fe_flood = ET.SubElement(filter_elem, 'feFlood')
        fe_flood.set('flood-color', color)
        fe_flood.set('result', 'floodOut')

        # 组合
        fe_composite1 = ET.SubElement(filter_elem, 'feComposite')
        fe_composite1.set('in', 'floodOut')
        fe_composite1.set('in2', 'blurOut')
        fe_composite1.set('operator', 'in')
        fe_composite1.set('result', 'shadowOut')

        # 合并原始图像和阴影
        fe_composite2 = ET.SubElement(filter_elem, 'feComposite')
        fe_composite2.set('in', 'SourceGraphic')
        fe_composite2.set('in2', 'shadowOut')
        fe_composite2.set('operator', 'over')

    def to_svg(self, text:str, **kwargs:Any) -> str:
        """生成SVG"""
        # 默认配置
        config = {
            'width': kwargs.get('width', None),
            'height': kwargs.get('height', None),
            'font_size': kwargs.get('font_size', 24),
            'font_family': kwargs.get('font_family', 'Arial'),
            'text_color': kwargs.get('text_color', 'black'),
            'bg_color': kwargs.get('bg_color', None),
            'align': kwargs.get('align', 'center'),
            'valign': kwargs.get('valign', 'middle'),
            'padding': kwargs.get('padding', 10),
            'font_weight': kwargs.get('font_weight', None),
            'font_style': kwargs.get('font_style', None),
            'letter_spacing': kwargs.get('letter_spacing', None),
            'multiline': kwargs.get('multiline', True),
            'line_height': kwargs.get('line_height', None),
            'text_shadow': kwargs.get('text_shadow', None),
        }

        # 根据文本长度自动计算尺寸
        if config['width'] is None:
            if '\n' in text:
                lines = text.split('\n')
                max_line = max(lines, key=len)
                config['width'] = len(max_line) * config['font_size'] * 0.7 + config['padding'] * 2
            else:
                config['width'] = len(text) * config['font_size'] * 0.7 + config['padding'] * 2

        if config['height'] is None:
            if '\n' in text and config['multiline']:
                line_count = text.count('\n') + 1
                line_height = config['line_height'] or config['font_size'] * 1.2
                config['height'] = config['padding'] * 2 + line_count * line_height
            else:
                config['height'] = config['font_size'] * 1.5 + config['padding'] * 2

        # 创建SVG
        svg = self.create_svg(config)

        # 添加文本
        self.add_text(svg, text, config)

        # 生成XML
        xml_str = ET.tostring(svg, encoding='utf-8')
        pretty_xml = minidom.parseString(xml_str).toprettyxml(indent='  ')

        # 删除XML声明
        pretty_xml = re.sub(r'<\?xml[^>]+\?>\n', '', pretty_xml)

    def save_to_file(self, text:str, output_file:str, **kwargs:Any) -> str:
        # 写入文件
        with open(output_file, "w", encoding="utf-8") as f:
            f.write(self.to_svg(text, **kwargs))
        return output_file

class label_gif_converter(any_class):
    def __init__(self):
        from PIL import Image, ImageDraw, ImageFont
        self.Image = Image
        self.ImageDraw = ImageDraw
        self.ImageFont = ImageFont


    def to_label_gif(self, text:str, output_file:str, **kwargs:Any) -> str:
        from PIL import Image, ImageDraw, ImageFont
        """
        将文本转换为GIF动画标签
        :param text: 要转换的文本
        :param output_file: 输出文件路径
        :param kwargs: 其他参数
            - font_size: 字体大小(默认24)
            - font_family: 字体名称(默认Arial)
            - text_color: 文本颜色(默认black)
            - bg_color: 背景颜色(默认white)
            - padding: 内边距(默认10)
            - duration: 每帧持续时间(默认50ms)
            - width: 输出图像宽度(可选)
            - height: 输出图像高度(可选)
        :return: 输出文件对象
        """
        # 配置参数
        config = {
            'font_size': kwargs.get('font_size', 24),
            'font_family': kwargs.get('font_family', 'Arial'),
            'text_color': kwargs.get('text_color', 'black'),
            'bg_color': kwargs.get('bg_color', 'white'),
            'padding': kwargs.get('padding', 10),
            'duration': kwargs.get('duration', 50),
            'width': kwargs.get('width', None),
            'height': kwargs.get('height', None)
        }

        # 创建字体对象
        try:
            font = self.ImageFont.truetype(config['font_family'], config['font_size'])
        except:
            font = self.ImageFont.load_default()

        # 计算文本尺寸
        dummy_draw = self.ImageDraw.Draw(self.Image.new('RGB', (1, 1)))
        text_bbox = dummy_draw.textbbox((0, 0), text, font=font)
        text_width = text_bbox[2] - text_bbox[0]
        text_height = text_bbox[3] - text_bbox[1]

        # 计算基础尺寸
        base_width = text_width * 3
        base_height = text_height + config['padding'] * 2

        # 如果指定了目标尺寸，计算缩放比例
        if config['width'] is not None and config['height'] is not None:
            target_width = config['width']
            target_height = config['height']
        else:
            target_width = base_width
            target_height = base_height

        frames: List[Image.Image] = []

        # 1. 文字逐渐生成阶段 (20帧)
        for i in range(20):
            image = self.Image.new('RGB', (base_width, base_height), config['bg_color'])
            draw = self.ImageDraw.Draw(image)

            # 逐字显示文本
            show_length = int((i + 1) / 20 * len(text))
            display_text = text[:show_length]

            # 在中心位置绘制
            x = base_width // 2 - text_width // 2
            y = config['padding']
            draw.text((x, y), display_text, font=font, fill=config['text_color'])

            # 缩放到目标尺寸
            image = image.resize((target_width, target_height), self.Image.Resampling.LANCZOS)
            frames.append(image)

        # 2. 向左移动和反弹阶段 (30帧)
        for i in range(30):
            image = self.Image.new('RGB', (base_width, base_height), config['bg_color'])
            draw = self.ImageDraw.Draw(image)

            # 使用正弦函数创建平滑的移动
            progress = i / 30
            x = base_width // 2 - text_width // 2 - text_width * math.sin(progress * math.pi)
            y = config['padding']

            draw.text((x, y), text, font=font, fill=config['text_color'])

            # 缩放到目标尺寸
            image = image.resize((target_width, target_height), self.Image.Resampling.LANCZOS)
            frames.append(image)

        # 3. 向右移动和反弹阶段 (30帧)
        for i in range(30):
            image = self.Image.new('RGB', (base_width, base_height), config['bg_color'])
            draw = self.ImageDraw.Draw(image)

            progress = i / 30
            x = base_width // 2 - text_width // 2 + text_width * math.sin(progress * math.pi)
            y = config['padding']

            draw.text((x, y), text, font=font, fill=config['text_color'])

            # 缩放到目标尺寸
            image = image.resize((target_width, target_height), self.Image.Resampling.LANCZOS)
            frames.append(image)

        # 4. 溶解效果 (20帧)
        import random
        char_positions = [(i, c) for i, c in enumerate(text)]
        for i in range(20):
            image = self.Image.new('RGB', (base_width, base_height), config['bg_color'])
            draw = self.ImageDraw.Draw(image)

            # 随机移除字符
            remaining_chars = len(text) - int((i + 1) / 20 * len(text))
            current_chars = random.sample(char_positions, remaining_chars)
            current_chars.sort()  # 保持字符顺序

            display_text = ''.join(c for _, c in current_chars)
            x = base_width // 2 - text_width // 2
            y = config['padding']

            draw.text((x, y), display_text, font=font, fill=config['text_color'])

            # 缩放到目标尺寸
            image = image.resize((target_width, target_height), self.Image.Resampling.LANCZOS)
            frames.append(image)

        # 保存为GIF
        frames[0].save(
            output_file,
            save_all=True,
            append_images=frames[1:],
            duration=config['duration'],
            loop=0
        )

        return output_file

class light_str(left_value_reference[str]):
    '''
    Support some function for one target string
    '''
    def __init__(self, s:Union[str, List[bytes]] = ""):
        if isinstance(s, str):
            super().__init__(s)
        elif isinstance(s, list):
            super().__init__(list_byte_to_string(s))
        else:
            raise TypeError(f"Unsupported type for light_str: {type(s)}")

    @property
    def content(self):
        return self.ref_value
    @content.setter
    def content(self, value:str):
        self.ref_value = value

    @property
    def length(self):
        return len(self.content)
    def __len__(self):
        return self.length()

    def append(self, s:Union[str, Any]):
        self.content += str(s)

    def clear(self):
        self.content = ""

    def insert(self, pos:int, s:Union[str, Any]):
        if pos < 0 or pos > self.length:
            raise IndexError("Position out of range")
        self.content = self.content[:pos] + str(s) + self.content[pos:]

    def erase(self, pos:int, length:int):
        if pos < 0 or pos + length > self.length:
            raise IndexError("Position and length out of range")
        self.content = self.content[:pos] + self.content[pos + length:]

    def find(self, s:Union[str, Any], pos:int=0):
        return self.content.find(str(s), pos)

    def substr(self, pos:int, length:int):
        if pos < 0 or pos + length > self.length:
            raise IndexError("Position and length out of range")
        return self.content[pos:pos + length]

    def replace(self, old:Union[str, Any], new:Union[str, Any]):
        self.content = self.content.replace(str(old), str(new))

    def split(self, sep:Union[str, Any]):
        return self.content.split(str(sep))

    def join_by_me(self, seq:Sequence[Union[str, Any]]):
        return self.content.join([str(item) for item in seq])

    def lower(self):
        return self.content.lower()

    def upper(self):
        return self.content.upper()

    def strip(self):
        return self.content.strip()
    def lstrip(self):
        return self.content.lstrip()
    def rstrip(self):
        return self.content.rstrip()

    def trim(self, *chars:str):
        return self.content.strip(*chars)
    def ltrim(self, *chars:str):
        return self.content.lstrip(*chars)
    def rtrim(self, *chars:str):
        return self.content.rstrip(*chars)

    def startswith(self, s:Union[str, Any]):
        return self.content.startswith(str(s))
    def endswith(self, s:Union[str, Any]):
        return self.content.endswith(str(s))
    def contains(self, s:Union[str, Any]):
        return str(s) in self.content
    def __contains__(self, s:Union[str, Any]):
        return self.contains(s)
    def is_empty(self):
        return self.content is None or self.content == ""

    def get_limit_str(self, length:int=50):
        if length < 0:
            raise ValueError("Length must be non-negative")
        return limit_str(self.content, length)
    def get_fill_str(self, length:int=50, fill:str=" ", side:str="right"):
        if length < 0:
            raise ValueError("Length must be non-negative")
        return fill_str(self.content, length, fill, side)

    def to_svg(self, *, output_file:Optional[str]=None, **kwargs:Any) -> str:
        '''
        当output_file为None时，返回svg字符串
        当output_file不为None时，保存svg到文件
        '''
        if output_file is None:
            return str2svg_converter().to_svg(self.content, **kwargs)
        else:
            return str2svg_converter().save_to_file(self.content, output_file, **kwargs)

    def to_label_gif(self, output_file:str, **kwargs:Any) -> str:
        '''
        将字符串转换为简单文字动画
        '''
        return label_gif_converter().to_label_gif(self.content, output_file, **kwargs)

static_is_enable_unwrapper_none2none = False
def enable_unwrapper_none2none():
    global static_is_enable_unwrapper_none2none
    static_is_enable_unwrapper_none2none = True
def disable_unwrapper_none2none():
    global static_is_enable_unwrapper_none2none
    static_is_enable_unwrapper_none2none = False

def UnWrapper(from_) -> str:
    if from_ is None:
        if static_is_enable_unwrapper_none2none:
            return "null"
        else:
            raise ValueError("None is not support")

    if isinstance(from_, str):
        return from_
    elif isinstance(from_, Path):
        return str(from_)
    elif isinstance(from_, IO):
        return from_.name

    ReEx = light_reflection(from_)
    if ReEx.contains_method("to_string"):
        return from_.to_string()
    elif ReEx.contains_method("__str__"):
        return str(from_)

    else:
        raise ValueError("Unsupport instance")

def Able_UnWrapper(from_) -> bool:
    if isinstance(from_, str):
        return True
    elif isinstance(from_, Path):
        return True

    ReEx = light_reflection(from_)
    if ReEx.contains_method("to_string"):
        return True
    elif ReEx.contains_method("__str__"):
        return True
    else:
        return False

def Combine(*args) -> str:
    result:str = ""
    if len(args) == 1:
        if isinstance(args[0], Sequence):
            for current in args:
                result += UnWrapper(current)
                result += ","
        else:
            result = UnWrapper(args[0])
    else:
        for current in args:
            result += UnWrapper(current)

try:
    import                                     jieba
    def word_segmentation(
        sentence:   Union[str, light_str, Any],
        cut_all:    bool                    = False,
        HMM:        bool                    = True,
        use_paddle: bool                    = False
        ) -> Sequence[Optional[Union[Any, str]]]:
        return jieba.dt.cut(UnWrapper(sentence), cut_all=cut_all, HMM=HMM, use_paddle=use_paddle)
except ImportError:
    def word_segmentation(*args, **kwargs):
        raise ValueError("jieba is not install")

if __name__ == "__main__":
    text = "/LiuBai/"
    converter = label_gif_converter()
    converter.to_label_gif(text, "output.gif",
        font_size=64, # 增大字体以适应更大的分辨率
        font_family="Arial Black", # 使用粗体字体
        text_color="black",
        bg_color="white",
        duration=50,
        resolution=300.0 # 设置更高的分辨率
    )
