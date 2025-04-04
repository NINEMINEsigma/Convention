import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from Convention.Web.Core import *

async def run():
    result = get_webpage_text("https://www.weather.com.cn/weather/101010100.shtml")
    print(result)

if __name__ == "__main__":
    asyncio.run(run())
