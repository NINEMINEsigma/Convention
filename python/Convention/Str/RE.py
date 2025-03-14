import re

class light_re:
    def __init__(self, pattern):
        """
        初始化正则表达式对象
        :param pattern: 正则表达式模式
        """
        self.pattern = pattern
        self.compiled_pattern = re.compile(pattern)

    def match(self, string):
        """
        尝试从字符串的起始位置匹配正则表达式
        :param string: 要匹配的字符串
        :return: 匹配结果，Match对象或None
        """
        return self.compiled_pattern.match(string)

    def search(self, string):
        """
        在字符串中搜索匹配正则表达式的部分
        :param string: 要搜索的字符串
        :return: 匹配结果，Match对象或None
        """
        return self.compiled_pattern.search(string)

    def find_all(self, string):
        """
        返回字符串中所有匹配正则表达式的部分
        :param string: 要搜索的字符串
        :return: 匹配结果列表
        """
        return self.compiled_pattern.findall(string)

    def sub(self, repl, string):
        """
        使用repl替换字符串中所有匹配正则表达式的部分
        :param repl: 替换字符串或替换函数
        :param string: 要替换的字符串
        :return: 替换后的字符串
        """
        return self.compiled_pattern.sub(repl, string)

def number():
    return r"\d"

def any():
    return r"."

def block():
    return r"[]"

def not_number():
    return r"\D"

def space():
    return r"\s"
def not_space():
    return r"\S"

def word():
    return r"\w"
def not_word():
    return r"\W"

def follow_possible():
    return r"*"
def follow_exist():
    return r"+"
def follow_once_or_not():
    return r"?"
def follow_muti_time(times):
    return r"{"+f"{times}"+r"}"
def follow_muti_time_range(min_times, max_times):
    return r"{"+f"{min_times},{max_times}"+r"}"

def begin():
    return r"^"
def end():
    return r"$"

def origin(tag):
    return r"\\"+tag

def or_opt(left, right):
    return left+r"|"+right

# 示例用法
if __name__ == "__main__":
    regex = light_re(number()+follow_exist())
    test_string = "这里有1个数字和456个数字"

    match_result = regex.match(test_string)
    print("Match:", match_result.group() if match_result else "No match")

    search_result = regex.search(test_string)
    print("Search:", search_result.group() if search_result else "No search result")

    findall_result = regex.find_all(test_string)
    print("Findall:", findall_result)

    sub_result = regex.sub("<number>", test_string)
    print("Sub:", sub_result)
