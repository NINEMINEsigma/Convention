from ...Internal                    import *
from ...File.Core                   import tool_file_or_str, UnWrapper as UnwrapperFile2Str
from llama_index.core.embeddings    import BaseEmbedding
from llama_index.core               import SimpleDirectoryReader
from llama_index.core.schema        import Document
from pydantic                       import Field
import requests                     as     requests
import asyncio                      as     asyncio
import aiohttp                      as     aiohttp

# https://docs.llamaindex.ai/en/stable/module_guides/loading/simpledirectoryreader/#simpledirectoryreader
def make_directory_reader(
    input_data:             Union[tool_file_or_str, List[tool_file_or_str]],
    exclude_data:           Optional[List[str]]                 = None,
    exclude_hidden:         bool                                = True,
    exclude_empty:          bool                                = False,
    errors:                 str                                 = "ignore",
    recursive:              bool                                = False,
    encoding:               str                                 = "utf-8",
    filename_as_id:         bool                                = False,
    required_extensions:    Optional[list[str]]                 = None,
    num_files_limit:        Optional[int]                       = None,
    file_metadata:          Optional[Callable[[str], dict]]     = None,
    raise_on_error:         bool                                = False,
    **kwargs
    ) -> SimpleDirectoryReader:
    '''
    """
    创建SimpleDirectoryReader实例。

    参数:
        input_data: 输入数据，可以是单个文件/目录路径或路径列表
        exclude: 要排除的文件glob模式列表
        exclude_hidden: 是否排除隐藏文件
        exclude_empty: 是否排除空文件
        errors: 编码错误处理方式
        recursive: 是否递归搜索子目录
        encoding: 文件编码
        filename_as_id: 是否使用文件名作为文档ID
        required_exts: 必需的文件扩展名列表
        num_files_limit: 读取文件数量限制
        file_metadata: 文件元数据提取函数
        raise_on_error: 读取失败时是否抛出异常
        kwargs: 其他参数

    返回:
        SimpleDirectoryReader实例
    """
    '''
    input_dir = None
    input_files = None
    if isinstance(input_data, tool_file_or_str):
        if input_data.is_dir():
            input_dir = input_data
        else:
            input_files = [input_data]
    elif isinstance(input_data, list):
        input_files = [file for file in input_data if isinstance(file, tool_file_or_str)]

    for i in range(len(required_extensions)):
        if required_extensions[i].startswith("."):
            required_extensions[i] = required_extensions[i][1:]

    return SimpleDirectoryReader(
        input_dir=input_dir,
        input_files= input_files,
        exclude=exclude_data,
        exclude_hidden=exclude_hidden,
        exclude_empty=exclude_empty,
        errors=errors,
        recursive=recursive,
        encoding=encoding,
        filename_as_id=filename_as_id,
        required_exts=required_extensions,
        num_files_limit=num_files_limit,
        file_metadata=file_metadata,
        raise_on_error=raise_on_error,
        **kwargs
    )

class EasyIndexReader(left_value_reference[SimpleDirectoryReader]):
    def __init__(self, reader: SimpleDirectoryReader|tool_file_or_str):
        super().__init__(
            reader
            if isinstance(reader, SimpleDirectoryReader)
            else SimpleDirectoryReader(UnwrapperFile2Str(reader))
            )

    @property
    def reader(self) -> SimpleDirectoryReader:
        return self.reader

    def load_data(self) -> List[Document]:
        return self.reader.load_data()

class LlamaCPPEmbedding(BaseEmbedding, any_class):
    """
    LlamaCPP嵌入类，用于生成文本嵌入向量。
    继承自BaseEmbedding基类。
    """

    model_uid: str = Field(
        default="unknown",
        description="要使用的LlamaCPP模型ID(或文件路径)。",
    )
    base_url: str = Field(
        default="http://127.0.0.1:8080",
        description="LlamaCPP服务的基础URL。",
    )
    timeout: float = Field(
        default=60.0,
        description="请求超时时间(秒)。",
    )

    def __init__(
        self,
        model_uid: str,
        base_url: str = "http://127.0.0.1:8080",
        timeout: float = 60.0,
        **kwargs: Any,
    ) -> None:
        """
        初始化LlamaCPP嵌入类。

        参数:
            model_uid: LlamaCPP模型ID(或文件路径)
            base_url: LlamaCPP服务URL,默认为http://127.0.0.1:8080
            timeout: 请求超时时间,默认60秒
            **kwargs: 其他参数
        """
        super().__init__(
            model_uid=model_uid,
            base_url=base_url,
            timeout=timeout,
            **kwargs,
        )

    @classmethod
    def class_name(cls) -> str:
        """返回类名"""
        return "LlamaCPPEmbedding"

    def _get_query_embedding(self, query: str) -> List[float]:
        """
        获取查询文本的嵌入向量。

        参数:
            query: 查询文本
        返回:
            嵌入向量列表
        """
        return self.get_general_text_embedding(query)

    async def _aget_query_embedding(self, query: str) -> List[float]:
        """
        异步获取查询文本的嵌入向量。

        参数:
            query: 查询文本
        返回:
            嵌入向量列表
        """
        return await self.aget_general_text_embedding(query)

    def _get_text_embedding(self, text: str) -> List[float]:
        """
        获取文本的嵌入向量。

        参数:
            text: 输入文本
        返回:
            嵌入向量列表
        """
        return self.get_general_text_embedding(text)

    async def _aget_text_embedding(self, text: str) -> List[float]:
        """
        异步获取文本的嵌入向量。

        参数:
            text: 输入文本
        返回:
            嵌入向量列表
        """
        return await self.aget_general_text_embedding(text)

    def _get_text_embeddings(self, texts: List[str]) -> List[List[float]]:
        """
        批量获取多个文本的嵌入向量。

        参数:
            texts: 文本列表
        返回:
            嵌入向量列表的列表
        """
        embeddings_list: List[List[float]] = []
        for text in texts:
            embeddings = self.get_general_text_embedding(text)
            embeddings_list.append(embeddings)
        return embeddings_list

    async def _aget_text_embeddings(self, texts: List[str]) -> List[List[float]]:
        """
        异步批量获取多个文本的嵌入向量。

        参数:
            texts: 文本列表
        返回:
            嵌入向量列表的列表
        """
        return await asyncio.gather(
            *[self.aget_general_text_embedding(text) for text in texts]
        )

    def get_general_text_embedding(self, prompt: str) -> List[float]:
        """
        调用LlamaCPP服务获取文本嵌入向量。

        参数:
            prompt: 输入文本
        返回:
            嵌入向量列表
        异常:
            如果API调用失败则抛出异常
        """
        headers = {"Content-Type": "application/json"}
        json_data = {"input": prompt, "model": self.model_uid}
        try:
            response = requests.post(
                url=f"{self.base_url}/embedding",
                headers=headers,
                json=json_data,
                timeout=self.timeout,
            )
            response.encoding = "utf-8"
            if response.status_code != 200:
                raise Exception(
                    f"LlamaCPP call failed with status code {response.status_code}."
                    f"Details: {response.text}"
                )

            # 打印响应内容以便调试
            response_json = response.json()
            if globals().get("__DEBUG__", False):
                print(f"API响应: {response_json}")

            # 尝试适应不同的响应格式
            if isinstance(response_json, list) and len(response_json) > 0:
                if "embedding" in response_json[0]:
                    if isinstance(response_json[0]["embedding"], list):
                        return response_json[0]["embedding"]
                    else:
                        return response_json[0]["embedding"][0]
            elif "data" in response_json and len(response_json["data"]) > 0:
                if "embedding" in response_json["data"][0]:
                    if isinstance(response_json["data"][0]["embedding"], list):
                        return response_json["data"][0]["embedding"]
                    else:
                        return response_json["data"][0]["embedding"][0]
            elif "embedding" in response_json:
                if isinstance(response_json["embedding"], list):
                    return response_json["embedding"]
                else:
                    return response_json["embedding"][0]

            # 如果以上格式都不匹配，尝试递归查找embedding字段
            def find_embedding(obj):
                if isinstance(obj, dict):
                    for key, value in obj.items():
                        if key == "embedding" and isinstance(value, list):
                            return value
                        result = find_embedding(value)
                        if result is not None:
                            return result
                elif isinstance(obj, list):
                    for item in obj:
                        result = find_embedding(item)
                        if result is not None:
                            return result
                return None

            embedding = find_embedding(response_json)
            if embedding is not None:
                return embedding

            raise Exception(f"无法解析嵌入响应: {response_json}")
        except Exception as e:
            print(f"获取嵌入向量时出错: {str(e)}")
            raise

    async def aget_general_text_embedding(self, prompt: str) -> List[float]:
        """
        异步调用LlamaCPP服务获取文本嵌入向量。

        参数:
            prompt: 输入文本
        返回:
            嵌入向量列表
        异常:
            如果API调用失败则抛出异常
        """
        headers = {"Content-Type": "application/json"}
        json_data = {"input": prompt, "model": self.model_uid}
        try:
            async with aiohttp.ClientSession() as session:
                async with session.post(
                    url=f"{self.base_url}/embedding",
                    headers=headers,
                    json=json_data,
                    timeout=self.timeout,
                ) as response:
                    if response.status != 200:
                        raise Exception(
                            f"LlamaCPP call failed with status code {response.status}."
                        )

                    data = await response.json()
                    # 打印响应内容以便调试
                    print(f"异步API响应: {data}")

                    # 尝试适应不同的响应格式
                    if isinstance(data, list) and len(data) > 0:
                        if "embedding" in data[0]:
                            if isinstance(data[0]["embedding"], list):
                                return data[0]["embedding"]
                            else:
                                return data[0]["embedding"][0]
                    elif "data" in data and len(data["data"]) > 0:
                        if "embedding" in data["data"][0]:
                            if isinstance(data["data"][0]["embedding"], list):
                                return data["data"][0]["embedding"]
                            else:
                                return data["data"][0]["embedding"][0]
                    elif "embedding" in data:
                        if isinstance(data["embedding"], list):
                            return data["embedding"]
                        else:
                            return data["embedding"][0]

                    # 如果以上格式都不匹配，尝试递归查找embedding字段
                    def find_embedding(obj):
                        if isinstance(obj, dict):
                            for key, value in obj.items():
                                if key == "embedding" and isinstance(value, list):
                                    return value
                                result = find_embedding(value)
                                if result is not None:
                                    return result
                        elif isinstance(obj, list):
                            for item in obj:
                                result = find_embedding(item)
                                if result is not None:
                                    return result
                        return None

                    embedding = find_embedding(data)
                    if embedding is not None:
                        return embedding

                    raise Exception(f"无法解析嵌入响应: {data}")
        except Exception as e:
            print(f"异步获取嵌入向量时出错: {str(e)}")
            raise

    def test_health(self) -> int:
        response = requests.get(f"{self.base_url}/health")
        return response.status_code

