from .Reflection    import *
from ..File.Core    import tool_file, tool_file_or_str, UnWrapper as UnwrapperFile2Str

class EasySaveSetting(BaseModel, any_class):
    # 从目标文件进行序列化/反序列化
    file:           str         = Field(description="目标文件")
    # 序列化/反序列化的格式方法
    format:         Literal["json", "binary"] = Field(description="保存模式", default="json")
    # TODO: refChain:       bool        = Field(description="是否以保留引用的方式保存", default=True)
    # 文件形式与参数
    # TODO: encoding:       str         = Field(description="编码", default="utf-8")
    # TODO: prettyPrint:    bool        = Field(description="是否缩进", default=True)
    is_backup:      bool        = Field(description="是否备份", default=True)
    backup_suffix:  str         = Field(description="备份后缀", default=".backup")
    # 序列化/反序列化时, 如果设置了忽略字段的谓词, 则被谓词选中的字段将不会工作
    # 如果设置了选择字段的谓词, 则被选中的字段才会工作
    ignore_pr:      Optional[Callable[[FieldInfo], bool]] = Field(description="忽略字段的谓词", default=None)
    select_pr:      Optional[Callable[[FieldInfo], bool]] = Field(description="选择字段的谓词", default=None)

class ESWriter(BaseModel, any_class):
    setting:            EasySaveSetting = Field(description="设置")

    @sealed
    def _GetFields(self, rtype:RefType) -> List[FieldInfo]:
        '''
        获取字段
        '''
        fields: List[FieldInfo] = []
        if self.setting.ignore_pr is not None and self.setting.select_pr is not None:
            fields = [ field for field in rtype.GetAllFields() if self.setting.select_pr(field) and not self.setting.ignore_pr(field) ]
        elif self.setting.select_pr is None and self.setting.ignore_pr is None:
            fields = rtype.GetFields()
        elif self.setting.ignore_pr is not None:
            fields = [ field for field in rtype.GetAllFields() if not self.setting.ignore_pr(field) ]
        else:
            fields = [ field for field in rtype.GetAllFields() if self.setting.select_pr(field) ]
        return fields

    @sealed
    def _DoJsonSerialize(self, result_file:tool_file, rtype:RefType, rinstance:Any) -> Any:
        '''
        序列化: json格式
        '''

        def dfs(rtype:RefType, rinstance:Any) -> Dict[str, Any]|Any:
            if rinstance is None:
                return rinstance

            if rtype.IsUnion:
                rtype = TypeManager.GetInstance().CreateOrGetRefType(rinstance)

            if rtype.IsValueType:
                return rinstance
            elif rtype.IsCollection:
                try:
                    if rtype.IsList:
                        return [ dfs(TypeManager.GetInstance().CreateOrGetRefType(iter_), iter_) for iter_ in rinstance ]
                    elif rtype.IsSet:
                        return { dfs(TypeManager.GetInstance().CreateOrGetRefType(iter_), iter_) for iter_ in rinstance }
                    elif rtype.IsTuple:
                        return tuple(dfs(TypeManager.GetInstance().CreateOrGetRefType(iter_), iter_) for iter_ in rinstance)
                    elif rtype.IsDictionary:
                        return { keyname:dfs(TypeManager.GetInstance().CreateOrGetRefType(iter_), iter_) for keyname, iter_ in rinstance.items() }
                except Exception as e:
                    raise ReflectionException(f"{ConsoleFrontColor.RED}容器<{rtype.RealType}>"\
                        f"在序列化时遇到错误:{ConsoleFrontColor.RESET}\n{e}") from e
                raise NotImplementedError(f"{ConsoleFrontColor.RED}不支持的容器: {rinstance}"\
                        f"<{rtype.print_str(verbose=GetInternalDebug())}>{ConsoleFrontColor.RESET}")
            else:
                fields: List[FieldInfo] = self._GetFields(rtype)
                layer:  Dict[str, Any]  = {
                    "__type": f"{rtype.RealType.__module__}.{rtype.RealType.__name__}, {rtype.RealType.__name__}",
                    "value": {}
                }
                for field in fields:
                    try:
                        layer["value"][field.FieldName] = dfs(
                            TypeManager.GetInstance().CreateOrGetRefType(field.FieldType),
                            field.GetValue(rinstance)
                            )
                    except Exception as e:
                        raise ReflectionException(f"{ConsoleFrontColor.RED}字段{field.FieldName}"\
                            f"<{field.FieldType}>在序列化时遇到错误:{ConsoleFrontColor.RESET}\n{e}") from e
                return layer

        result_file.data = {
            "__type": f"{rtype.RealType.__module__}.{rtype.RealType.__name__}, {rtype.RealType.__name__}",
            "value": dfs(rtype, rinstance)
        }
        result_file.save_as_json()

    @sealed
    def _DoBinarySerialize(self, result_file:tool_file, rinstance:Any) -> Any:
        '''
        序列化: 二进制格式
        '''
        result_file.data = rinstance
        result_file.save_as_binary()

    @virtual
    def Serialize(self, result_file:tool_file, rtype:RefType, rinstance:Any) -> Any:
        '''
        序列化
        '''
        if self.setting.format == "json":
            self._DoJsonSerialize(result_file, rtype, rinstance)
        elif self.setting.format == "binary":
            self._DoBinarySerialize(result_file, rinstance)
        else:
            raise NotImplementedError(f"不支持的格式: {self.setting.format}")

    @virtual
    def Write[T](self, rinstance:T) -> tool_file:
        '''
        写入数据
        '''
        result_file:    tool_file   = tool_file(self.setting.file)
        backup_file:    tool_file   = None
        if result_file.dirpath is not None and not tool_file(result_file.dirpath).exists():
            raise FileNotFoundError(f"文件路径不存在: {result_file.dirpath}")
        if result_file.exists() and self.setting.is_backup:
            if result_file.dirpath is not None:
                backup_file = result_file.dirpath | (result_file.get_filename(True) + self.setting.backup_suffix)
            else:
                backup_file = tool_file(result_file.get_filename(True) + self.setting.backup_suffix)
            result_file.copy(backup_file)
        try:
            self.Serialize(result_file, TypeManager.GetInstance().CreateOrGetRefType(rinstance), rinstance)
        except Exception as e:
            if backup_file is not None:
                result_file.remove()
                backup_file.copy(result_file)
                backup_file.remove()
            raise e
        finally:
            if backup_file is not None:
                backup_file.remove()
        return result_file

class ESReader(BaseModel, any_class):
    setting:            EasySaveSetting = Field(description="设置")

    @sealed
    def _GetFields(self, rtype:RefType) -> List[FieldInfo]:
        '''
        获取字段
        '''
        fields: List[FieldInfo] = []
        if self.setting.ignore_pr is not None and self.setting.select_pr is not None:
            fields = [ field for field in rtype.GetAllFields() if self.setting.select_pr(field) and not self.setting.ignore_pr(field) ]
        elif self.setting.select_pr is None and self.setting.ignore_pr is None:
            fields = rtype.GetFields()
        elif self.setting.ignore_pr is not None:
            fields = [ field for field in rtype.GetAllFields() if not self.setting.ignore_pr(field) ]
        else:
            fields = [ field for field in rtype.GetAllFields() if self.setting.select_pr(field) ]
        return fields

    def get_rtype_from_typen(self, type_label:str) -> RefType:
        '''
        从类型标签中获取类型
        '''
        try:
            return TypeManager.GetInstance().CreateOrGetRefType(get_type_from_string(type_label.split(",")[1].strip()))
        except Exception:
            module_name, _, class_name = type_label.split(",")[0].strip().rpartition('.')
            return TypeManager.GetInstance().CreateOrGetRefType(get_type_from_string_with_module(class_name, module_name))

    @sealed
    def _DoJsonDeserialize(self, read_file:tool_file, rtype:Optional[RTypen[Any]] = None) -> Any:
        '''
        反序列化: json格式

        Args:
            read_file (tool_file): 要读取的文件对象
            rtype (Optional[RTypen[Any]], optional): 目标类型. 如果为None, 则从文件中读取类型信息. Defaults to None.

        Returns:
            Any: 反序列化后的对象

        Raises:
            NotImplementedError: 当遇到不支持的集合类型时抛出
            ValueError: 当rinstance不为None时抛出
        '''
        # 从文件中加载JSON数据
        layers:             Dict[str, Any]  = read_file.load_as_json()
        result_instance:    Any             = None

        # 如果未指定类型, 则从JSON数据中获取类型信息
        if rtype is None:
            rtype = self.get_rtype_from_typen(layers["__type"])

        def dfs(rtype:Optional[RefType], layer:Dict[str, Any]|Any, rinstance:Any) -> Any:
            '''
            深度优先遍历反序列化

            Args:
                rtype (Optional[RefType]): 当前处理的类型
                layer (Dict[str, Any]|Any): 当前处理的JSON数据层
                rinstance (Any): 当前处理的对象实例

            Returns:
                Any: 反序列化后的对象
            '''
            # 如果类型为None且当前层包含类型信息, 则获取类型
            if rtype is None and isinstance(layer, dict) and "__type" in layer:
                rtype = self.get_rtype_from_typen(layer["__type"])
            if GetInternalDebug():
                print_colorful(ConsoleFrontColor.RED, f"layer: {layer}, rtype: {rtype.RealType}")

            # 处理值类型
            if rtype.IsValueType:
                return layer
            # 处理集合类型
            elif rtype.IsCollection:
                if rtype.IsList and isinstance(layer, list):
                    return [ dfs(None, iter_, None) for iter_ in layer ]
                elif rtype.IsSet and isinstance(layer, set):
                    return { dfs(None, iter_, None) for iter_ in layer }
                elif rtype.IsTuple and isinstance(layer, tuple):
                    return tuple(dfs(None, iter_, None) for iter_ in layer)
                elif rtype.IsDictionary and isinstance(layer, dict):
                    return { keyname:dfs(None, iter_, None) for keyname, iter_ in layer.items() }
                else:
                    raise NotImplementedError(f"未知的集合类型: {rtype.RealType}")
            # 处理对象类型
            else:
                # 获取所有字段
                fields: List[FieldInfo] = self._GetFields(rtype)
                # 创建实例
                if rinstance is None:
                    rinstance = rtype.CreateInstance()
                else:
                    raise ValueError(f"rinstance is not None")
                # 递归处理每个字段
                for field in fields:
                    field.SetValue(
                        rinstance,
                        dfs(TypeManager.GetInstance().CreateOrGetRefType(field.FieldType), layer["value"][field.FieldName], None)
                        )
                return rinstance

        # 从根节点开始反序列化
        result_instance = dfs(rtype, layers["value"], rtype.CreateInstance())
        return result_instance

    @sealed
    def _DoBinaryDeserialize(self, read_file:tool_file, rtype:RefType) -> Any:
        '''
        反序列化: 二进制格式
        '''
        return read_file.load_as_binary()

    @virtual
    def Deserialize(self, read_file:tool_file, rtype:Optional[RTypen[Any]]=None) -> Any:
        '''
        反序列化
        '''
        if self.setting.format == "json":
            return self._DoJsonDeserialize(read_file, rtype)
        elif self.setting.format == "binary":
            return self._DoBinaryDeserialize(read_file, rtype)
        else:
            raise NotImplementedError(f"不支持的格式: {self.setting.format}")

    @virtual
    def Read[T](self, rtype:Optional[RTypen[T]]=None) -> T:
        '''
        读取数据
        '''
        read_file: tool_file = tool_file(self.setting.file)
        if not read_file.exists():
            raise FileNotFoundError(f"文件不存在: {read_file}")
        if read_file.is_dir():
            raise IsADirectoryError(f"文件是目录: {read_file}")
        return self.Deserialize(read_file, rtype)

class EasySave(any_class):
    @staticmethod
    def Write[T](rinstance:T, file:tool_file_or_str=None, *, setting:Optional[EasySaveSetting]=None) -> tool_file:
        '''
        写入数据
        '''
        return ESWriter(setting=(setting if setting is not None else EasySaveSetting(file=UnwrapperFile2Str(file)))).Write(rinstance)

    @staticmethod
    def Read[T](file:tool_file_or_str=None, *, rtype:Optional[RTypen[T]]=None, setting:Optional[EasySaveSetting]=None) -> T:
        '''
        读取数据
        '''
        return ESReader(setting=(setting if setting is not None else EasySaveSetting(file=UnwrapperFile2Str(file)))).Read(rtype)
