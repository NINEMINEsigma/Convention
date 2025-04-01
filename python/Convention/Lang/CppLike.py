from ..Internal import *

# string

string = str
def to_string(target) -> str:
    return str(target)

# make
def make_tuple(*args) -> tuple:
    return args
def make_pair(first, second) -> tuple:
    return (first, second)
def make_list(*args) -> list:
    result:list = []
    for i in args:
        result.append(i)
    return result
def make_dict(*args, **kwargs) -> dict:
    result:dict = {}
    index = 0
    for i in args:
        result[index] = i
        index += 1
    for key in kwargs:
        result[key] = kwargs[key]
    return result
def make_map(*args, **kwargs) -> Dict[str, Any]:
    result:dict = {}
    index = 0
    for i in args:
        result[to_string(index)] = i
        index += 1
    for key in kwargs:
        result[to_string(key)] = kwargs[key]
    return result

def max_element(iter:Sequence[Any]) -> Any:
    max_value = iter[0]
    max_index = 0
    for index, value in range(iter):
        if value > max_value:
            max_value = value
            max_index = index
    return max_index
def min_element(iter:Sequence[Any]) -> Any:
    min_value = iter[0]
    min_index = 0
    for index, value in range(iter):
        if value < min_value:
            min_value = value
            min_index = index
    return min_index
def upper_boundary(iter:Sequence[Any]) -> int:
    return max(iter)
def lower_boundary(iter:Sequence[Any]) -> int:
    return min(iter)

