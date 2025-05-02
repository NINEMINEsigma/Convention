import multiprocessing.shared_memory
from ..Internal import *
import                 threading
import                 multiprocessing
import                 concurrent.futures
from functools  import wraps
import                 random

class ThreadCore(left_value_reference[threading.Thread]):
    def __init__(self, target: Callable, *args, **kwargs):
        thread = threading.Thread(target=target, args=args, kwargs=kwargs)
        super().__init__(thread)
        self._lock = threading.Lock()
        self._event = threading.Event()
        self._result = None
        self._exception = None

    def start(self) -> None:
        """启动线程"""
        self.ref_value.start()

    def join(self, timeout: float = None) -> None:
        """等待线程结束"""
        self.ref_value.join(timeout)

    def is_alive(self) -> bool:
        """检查线程是否存活"""
        return self.ref_value.is_alive()

    def set_result(self, result: Any) -> None:
        """设置线程执行结果"""
        with self._lock:
            self._result = result

    def get_result(self) -> Any:
        """获取线程执行结果"""
        with self._lock:
            return self._result

    def set_exception(self, exc: Exception) -> None:
        """设置线程异常"""
        with self._lock:
            self._exception = exc

    def get_exception(self) -> Exception:
        """获取线程异常"""
        with self._lock:
            return self._exception

    def wait(self, timeout: float = None) -> bool:
        """等待事件"""
        return self._event.wait(timeout)

    def set(self) -> None:
        """设置事件"""
        self._event.set()

    def clear(self) -> None:
        """清除事件"""
        self._event.clear()

    @staticmethod
    def synchronized(func: Callable) -> Callable:
        """线程同步装饰器"""
        @wraps(func)
        def wrapper(*args, **kwargs):
            with threading.Lock():
                return func(*args, **kwargs)
        return wrapper

    @staticmethod
    def thread_pool(max_workers: int = None) -> concurrent.futures.ThreadPoolExecutor:
        """创建线程池"""
        return concurrent.futures.ThreadPoolExecutor(max_workers=max_workers)

class ProcessCore(left_value_reference[multiprocessing.Process]):
    def __init__(self, target: Callable, *args, **kwargs):
        process = multiprocessing.Process(target=target, args=args, kwargs=kwargs)
        super().__init__(process)
        self._queue = multiprocessing.Queue()
        self._pipe = multiprocessing.Pipe()
        self._shared_value = multiprocessing.Value('i', 0)
        self._shared_array = multiprocessing.Array('i', 10)

    def start(self) -> None:
        """启动进程"""
        self.ref_value.start()

    def join(self, timeout: float = None) -> None:
        """等待进程结束"""
        self.ref_value.join(timeout)

    def is_alive(self) -> bool:
        """检查进程是否存活"""
        return self.ref_value.is_alive()

    def terminate(self) -> None:
        """终止进程"""
        self.ref_value.terminate()

    def kill(self) -> None:
        """强制终止进程"""
        self.ref_value.kill()

    def send(self, data: Any) -> None:
        """通过管道发送数据"""
        self._pipe[0].send(data)

    def receive(self) -> Any:
        """通过管道接收数据"""
        return self._pipe[1].recv()

    def put(self, item: Any) -> None:
        """向队列放入数据"""
        self._queue.put(item)

    def get(self) -> Any:
        """从队列获取数据"""
        return self._queue.get()

    def set_shared_value(self, value: int) -> None:
        """设置共享值"""
        with self._shared_value.get_lock():
            self._shared_value.value = value

    def get_shared_value(self) -> int:
        """获取共享值"""
        return self._shared_value.value

    def set_shared_array(self, index: int, value: int) -> None:
        """设置共享数组元素"""
        self._shared_array[index] = value

    def get_shared_array(self, index: int) -> int:
        """获取共享数组元素"""
        return self._shared_array[index]

    @staticmethod
    def shared_memory(name: str, create: bool = False, size: int = 0) -> multiprocessing.shared_memory.SharedMemory:
        """创建或访问共享内存"""
        try:
            if create:
                return multiprocessing.shared_memory.SharedMemory(name=name, create=True, size=size)
            return multiprocessing.shared_memory.SharedMemory(name=name)
        except FileNotFoundError:
            raise ValueError(f"找不到共享内存 '{name}'")
        except Exception as e:
            raise RuntimeError(f"访问共享内存时出错: {e}")

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

def run_until_complete(coro: Coroutine) -> Any:
    """Gets an existing event loop to run the coroutine.

    If there is no existing event loop, creates a new one.
    """
    import asyncio
    try:
        # Check if there's an existing event loop
        loop = asyncio.get_event_loop()

        # If we're here, there's an existing loop but it's not running
        return loop.run_until_complete(coro)

    except RuntimeError:
        # If we can't get the event loop, we're likely in a different thread, or its already running
        try:
            return asyncio.run(coro)
        except RuntimeError:
            raise RuntimeError(
                "Detected nested async. Please use nest_asyncio.apply() to allow nested event loops."
                "Or, use async entry methods like `aquery()`, `aretriever`, `achat`, etc."
            )

def run_async_coroutine(coro: Coroutine) -> Any:
    import asyncio
    try:
        # Check if there's an existing event loop
        loop = asyncio.get_event_loop()

        # If we're here, there's an existing loop but it's not running
        return loop.create_task(coro)

    except RuntimeError:
        # If we can't get the event loop, we're likely in a different thread, or its already running
        try:
            return asyncio.run(coro)
        except RuntimeError:
            raise RuntimeError(
                "Detected nested async. Please use nest_asyncio.apply() to allow nested event loops."
                "Or, use async entry methods like `aquery()`, `aretriever`, `achat`, etc."
            )

# 动态库加载

try:
    import ctypes
    
    class DynamicFunctionInfo(left_value_reference[ctypes._FuncPointer]):
        def __init__(self, 
                     func: ctypes._FuncPointer,
                     name:      str, 
                     argtypes:  List[Type[ctypes._CDataMeta]], 
                     restype:   Type[ctypes._CDataMeta]
                     ) -> None:
            super().__init__(func)
            self._name = name
            self._argtypes = argtypes
            self._restype = restype
            
        @property
        def name(self) -> str:
            return self._name
        
        @property
        def argtypes(self) -> List[Type[ctypes._CDataMeta]]:
            return self._argtypes
        
        @property
        def restype(self) -> Type[ctypes._CDataMeta]:
            return self._restype 
            
        def __call__(self, *args):
            for index, arg in enumerate(args):
                args[index] = self.argtypes[index](arg)
            return ctypes.cast(self.ref_value(*args), self.restype)
    
    class DynamicLibrary(left_value_reference[ctypes.CDLL]):
        '''
    示例:
    ---
        C++部分
        ```c++
    #include <iostream>
    
    extern "C" {
        int add(int a, int b) {
            return a + b;
        }
        
        void print_hello() {
            std::cout << "Hello from C++!" << std::endl;
        }
    }
        
        ```
        python调用
        ```python
    import ctypes
    
    # 加载动态链接库
    # 注意：在Windows上使用.dll，在Linux上使用.so
    lib = ctypes.CDLL('./example.dll')  # Windows
    # lib = ctypes.CDLL('./libexample.so')  # Linux
    
    # 调用C++函数
    result = lib.add(5, 3)
    print(f"5 + 3 = {result}")
    
    # 调用无返回值的函数
    lib.print_hello()
        ```
        
    ##    定义结构
        ```python
    import ctypes
    
    # 定义C结构体
    class Point(ctypes.Structure):
        _fields_ = [("x", ctypes.c_int),
                    ("y", ctypes.c_int)]
    
    # 加载库
    lib = ctypes.CDLL('./example.dll')
    
    # 设置函数参数和返回类型
    lib.create_point.argtypes = [ctypes.c_int, ctypes.c_int]
    lib.create_point.restype = ctypes.POINTER(Point)
    
    # 调用函数
    point = lib.create_point(10, 20)
    print(f"Point: ({point.contents.x}, {point.contents.y})")
        ```
        
        '''
        
        def __init__(self, path: str):
            self.path = ctypes.CDLL(path)
            self.lib = ctypes.CDLL(path)
    
        def __getattr__(self, item: str) -> Any:
            return getattr(self.lib, item)
    
        def SetupFunction(self, 
                          name:      str, 
                          argtypes:  List[Type[ctypes._CDataMeta]], 
                          restype:   Type[ctypes._CDataMeta]
                          ) -> DynamicFunctionInfo:
            '''
            设置函数参数和返回类型
            '''
            func = getattr(self.lib, name)
            func.argtypes = argtypes
            func.restype = restype
            
            return DynamicFunctionInfo(func, name, argtypes, restype)
            
        def CallFunction(self, name: str, *args) -> Any:
            '''
            调用函数
            '''
            return getattr(self.lib, name)(*args)
except:
    pass
    