from ..Workflow import *

# region ToString
def ToString(obj:Any) -> str:
    return {
        "result": Unwrapper2Str(obj)
    }
_ToString = WorkflowActionWrapper(ToString.__name__, ToString, "转换为字符串",
                                  {"obj": "Any"},
                                  {"result": "str"})
# endregion

# region ParseNumber
def ParseNumber(text:str) -> float:
    return {
        "result": float(text)
    }
_ParseNumber = WorkflowActionWrapper(ParseNumber.__name__, ParseNumber, "转换为数字",
                                     {"text": "str"},
                                     {"result": "float"})
# endregion

# region ParseInt
def ParseInt(text:str) -> int:
    return {
        "result": int(text)
    }
_ParseInt = WorkflowActionWrapper(ParseInt.__name__, ParseInt, "转换为整数",
                                  {"text": "str"},
                                  {"result": "int"})
# endregion

# region Replace
def Replace(text:str, old:str, new:str) -> str:
    return {
        "result": text.replace(old, new)
    }
_Replace = WorkflowActionWrapper(Replace.__name__, Replace, "替换",
                                 {"text": "str", "old": "str", "new": "str"},
                                 {"result": "str"})
# endregion

# region Split
def Split(text:str, sep:str) -> List[str]:
    return {
        "result": text.split(sep)
    }
_Split = WorkflowActionWrapper(Split.__name__, Split, "分割",
                               {"text": "str", "sep": "str"},
                               {"result": "Array"})
# endregion

# region Join
def Join(text:List[str|Any], sep:str) -> str:
    return sep.join([Unwrapper2Str(item) for item in text])
_Join = WorkflowActionWrapper(Join.__name__, Join, "连接",
                              {"text": "Array", "sep": "str"},
                              {"result": "str"})
# endregion

# region Remove
def Remove(text:str, target:str) -> str:
    return {
        "result": text.replace(target, "")
    }
_Remove = WorkflowActionWrapper(Remove.__name__, Remove, "删除",
                                {"text": "str", "target": "str"},
                                {"result": "str"})
# endregion

# region Format
def Format(text:str, target:str, mapper:str) -> str:
    return {
        "result": text.replace("{"+target+"}", mapper)
    }
_Format = WorkflowActionWrapper(Format.__name__, Format, "格式化",
                                {"text": "str", "target": "str", "mapper": "str"},
                                {"result": "str"})
# endregion

# region StartsWith
def StartsWith(text:str, target:str) -> bool:
    return {
        "result": text.startswith(target)
    }
_StartsWith = WorkflowActionWrapper(StartsWith.__name__, StartsWith, "以...开头",
                                    {"text": "str", "target": "str"},
                                    {"result": "bool"})
# endregion

# region EndsWith
def EndsWith(text:str, target:str) -> bool:
    return {
        "result": text.endswith(target)
    }
_EndsWith = WorkflowActionWrapper(EndsWith.__name__, EndsWith, "以...结尾",
                                  {"text": "str", "target": "str"},
                                  {"result": "bool"})
# endregion

# region Contains
def Contains(text:str, target:str) -> bool:
    return {
        "result": target in text
    }
_Contains = WorkflowActionWrapper(Contains.__name__, Contains, "包含",
                                  {"text": "str", "target": "str"},
                                  {"result": "bool"})
# endregion

# region IndexOf
def IndexOf(text:str, target:str) -> int:
    return {
        "result": text.find(target)
    }
_IndexOf = WorkflowActionWrapper(IndexOf.__name__, IndexOf, "查找",
                                 {"text": "str", "target": "str"},
                                 {"result": "int"})
# endregion

# region LastIndexOf
def LastIndexOf(text:str, target:str) -> int:
    return {
        "result": text.rfind(target)
    }
_LastIndexOf = WorkflowActionWrapper(LastIndexOf.__name__, LastIndexOf, "查找最后一个",
                                     {"text": "str", "target": "str"},
                                     {"result": "int"})
# endregion

# region Substring
def Substring(text:str, start:int, end:int) -> str:
    return {
        "result": text[start:end]
    }
_Substring = WorkflowActionWrapper(Substring.__name__, Substring, "子串",
                                   {"text": "str", "start": "int", "end": "int"},
                                   {"result": "str"})
# endregion

# region Trim
def Trim(text:str) -> str:
    return {
        "result": text.strip()
    }
_Trim = WorkflowActionWrapper(Trim.__name__, Trim, "去除前后空格",
                              {"text": "str"},
                              {"result": "str"})
# endregion

# region Boundary
def BoundaryMatching(text:str, front:str, back:str) -> str:
    front_index = text.find(front)
    back_index = text.rfind(back)
    if front_index == -1 or back_index == -1:
        return ""
    if front_index > back_index:
        return ""
    return {
        "result": text[(front_index+len(front)):back_index]
    }
_BoundaryMatching = WorkflowActionWrapper(BoundaryMatching.__name__, BoundaryMatching, "边界匹配",
                                          {"text": "str", "front": "str", "back": "str"},
                                          {"result": "str"})
# endregion

# region Partition
def Partition(text:str, sep:str) -> Tuple[str, str, str]:
    left, suffix, right = text.partition(sep)
    if suffix == "":
        return "", "", ""
    return {
        "left": left,
        "suffix": suffix,
        "right": right
    }
_Partition = WorkflowActionWrapper(Partition.__name__, Partition, "分割",
                                   {"text": "str", "sep": "str"},
                                   {
                                       "left": "str",
                                       "suffix": "str",
                                       "right": "str"
                                    })
# endregion

# region Rpartition
def Rpartition(text:str, sep:str) -> Tuple[str, str, str]:
    left, suffix, right = text.rpartition(sep)
    if suffix == "":
        return "", "", ""
    return {
        "left": left,
        "suffix": suffix,
        "right": right
    }
_Rpartition = WorkflowActionWrapper(Rpartition.__name__, Rpartition, "反向分割",
                                   {"text": "str", "sep": "str"},
                                   {
                                       "left": "str",
                                       "suffix": "str",
                                       "right": "str"
                                    })
# endregion

# region IsNullOrEmpty
def IsNullOrEmpty(text:str) -> bool:
    return {
        "result": text == "" or text is None
    }
_IsNullOrEmpty = WorkflowActionWrapper(IsNullOrEmpty.__name__, IsNullOrEmpty, "是否为空",
                                        {"text": "str"},
                                        {"result": "bool"})
# endregion

# region JsonSerialize
def JsonSerialize(obj:Any) -> str:
    import json
    return json.dumps(obj)
_JsonSerialize = WorkflowActionWrapper(JsonSerialize.__name__, JsonSerialize, "序列化",
                                       {"obj": "Any"},
                                       {"result": "str"})
# endregion

# region JsonDeserialize
def JsonDeserialize(text:str) -> Any:
    import json
    return json.loads(text)
_JsonDeserialize = WorkflowActionWrapper(JsonDeserialize.__name__, JsonDeserialize, "反序列化",
                                        {"text": "str"},
                                        {"result": "Any"})
# endregion

# region Limit
def Limit(text:str, length:int) -> str:
    from ..Str.Core import limit_str
    return limit_str(text, length)
_Limit = WorkflowActionWrapper(Limit.__name__, Limit, "限制长度",
                               {"text": "str", "length": "int"},
                               {"result": "str"})
# endregion



