# -*- coding: utf-8 -*-

from typing                     import *
from ..Internal             import *
import                                 random
from pydantic  import BaseModel as     Base

#不要继承这个类, 直到pydantic将私有变量相关的__pydantic_private__错误修复为止
#除非继承的类不打算使用私有或者保护变量
class BaseBehaviour(Base):
    pass

#一些工具
def split_elements(
    input:      list,
    *,
    ratios:     List[float]                                 = [1,1],
    pr:         Optional[Callable[[Any], bool]]             = None,
    shuffler:   Optional[Callable[[List[Any]], None]]       = None,
    ):
    if pr is not None:
        input:          list            = list(filter(pr, input))
    input_count:        int             = len(input)

    # 计算总比例
    total_ratio:        int             = sum(ratios)

    # 计算每个子集的比例
    split_indices:      List[int]       = []
    cumulative_ratio:   int             = 0
    for ratio in ratios:
        cumulative_ratio += ratio
        split_indices.append(int(input_count * (cumulative_ratio / total_ratio)))

    # 处理列表, 默认随机
    if shuffler is not None:
        shuffler(input)
    else:
        random.shuffle(input)

    # 划分
    result:             List[list]      = []
    start_index:        int             = 0
    for end_index in split_indices:
        result.append(input[start_index:end_index])
        start_index = end_index

    # 如果有剩余的，分配为最后一部分
    if start_index < len(input):
        result.append(input[start_index:])

    return result

def space_lines(line:int = 1):
    print('\n' * line, end="")



