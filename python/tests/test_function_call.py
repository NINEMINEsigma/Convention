from ..Convention.Internal import *
from ..Convention.LLM.LlamaIndex.Core import *
from ..Convention.LLM.LlamaIndex.Extensions import *
from ..Convention.Web.Core import get_webpage_text

# 创建LLM实例
llm = LLMObject(HttpLlamaCPP("http://10.10.230.60:61111", 120))
llm.set_as_global_llm()

# 定义工具函数
def get_weather(location: str) -> str:
    """获取指定地点的天气信息"""
    url = f"https://www.weather.com/weather/today/l/{location}"
    return get_webpage_text(url)

def get_stock_price(symbol: str) -> str:
    """获取股票价格"""
    url = f"https://finance.yahoo.com/quote/{symbol}"
    return get_webpage_text(url)

# 创建工具列表
tools = [
    {
        "name": "get_weather",
        "description": "获取指定地点的天气信息",
        "parameters": {
            "type": "object",
            "properties": {
                "location": {
                    "type": "string",
                    "description": "地点名称"
                }
            },
            "required": ["location"]
        }
    },
    {
        "name": "get_stock_price",
        "description": "获取股票价格",
        "parameters": {
            "type": "object",
            "properties": {
                "symbol": {
                    "type": "string",
                    "description": "股票代码"
                }
            },
            "required": ["symbol"]
        }
    }
]

def run():
    # 创建ReActAgent实例
    agent = ReActAgentCore(tools, verbose=True)
    
    # 测试天气查询
    print("测试天气查询:")
    response = agent.chat("今天珠海的天气怎么样")
    print(response)
    
    # 测试股票查询
    print("\n测试股票查询:")
    response = agent.chat("苹果公司的股票价格是多少")
    print(response)

if __name__ == "__main__":
    run() 