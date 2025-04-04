import time
import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

# from Convention.Algorithm.Graph.LazyThetaStar import *

# def main():
#     """主程序入口"""
#     # 创建可视化实例
#     vis = Visualization(width=1000, height=700, grid_size=40, cell_size=15)

#     # 运行可视化程序
#     vis.run()

from Convention.Lang.Reflection import *

class itest:
    a = 1

    def __init__(self):
        self.b = 2

    def inst_method(self):
        print("inst_method")

    @staticmethod
    def static_method():
        print("static_method")

    @classmethod
    def class_method(cls):
        print("class_method")

    @property
    def prop(self):
        print("prop")


def main():
    # print_colorful(ConsoleFrontColor.GREEN, itest.__dict__)
    # print_colorful(ConsoleFrontColor.RED, itest().__dict__)
    # print_colorful(ConsoleFrontColor.BLUE, type(itest()).__dict__)
    # print_colorful(ConsoleFrontColor.YELLOW, inspect.getmembers(itest))
    FieldInfos:List[FieldInfo] = []
    MethodInfos:List[MethodInfo] = []

    class_vars = itest.__dict__

    for name, member in inspect.getmembers(itest):
        if inspect.ismethod(member) or inspect.isfunction(member):
            # 获取方法签名
                sig = inspect.signature(member)
                is_static = inspect.isfunction(member)
                is_not_static = inspect.ismethod(member)
                if is_static == is_not_static:
                    raise ValueError(f"Method {name} is both static and not static")
                is_public = (name.startswith("__") and name.endswith("__")) or not name.startswith('_')

                # 构建参数列表
                parameters = []
                positional_parameters = []
                keyword_parameters = []

                for param_name, param in sig.parameters.items():
                    if param_name == 'self':
                        continue

                    param_info = ParameterInfo(
                        type = param.annotation if param.annotation != inspect.Parameter.empty else Any,
                        name = param_name,
                        is_optional = param.default != inspect.Parameter.empty,
                        default_value = param.default if param.default != inspect.Parameter.empty else None
                    )

                    parameters.append(param_info)
                    if param.kind == inspect.Parameter.POSITIONAL_ONLY or param.kind == inspect.Parameter.POSITIONAL_OR_KEYWORD:
                        positional_parameters.append(param_info)
                    elif param.kind == inspect.Parameter.KEYWORD_ONLY:
                        keyword_parameters.append(param_info)

                # 构建方法信息
                method_info = MethodInfo(
                    return_type = sig.return_annotation if sig.return_annotation != inspect.Signature.empty else Any,
                    parameters = parameters,
                    positional_parameters = positional_parameters,
                    keyword_parameters = keyword_parameters,
                    name = name,
                    ctype = type,
                    is_static = is_static,
                    is_public = is_public
                )
                MethodInfos.append(method_info)
        else:
            is_static = name in class_vars
            is_public = not name.startswith('_')
            field_info = FieldInfo(
                fieldType = type(member),
                name = name,
                ctype = type,
                is_static = is_static,
                is_public = is_public
            )
            FieldInfos.append(field_info)
    print_colorful(ConsoleFrontColor.GREEN, FieldInfos)
    print_colorful(ConsoleFrontColor.RED, MethodInfos)
    print_colorful(ConsoleFrontColor.BLUE, FieldInfos[0].MemberName)


if __name__ == "__main__":
    main()