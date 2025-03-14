from pathlib import Path
from ..Internal                 import *
from ..Lang.Reflection          import light_reflection

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

