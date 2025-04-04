from .Core                  import *

class WebTool:
    @staticmethod
    async def search(
        url:            Optional[str] = None, 
        query:          Optional[str] = None, 
        num_results:    int = 10, 
        **kwargs) -> str:
        '''
        use the {url} to search the first {num_results} result on the web for the query {query}

        url is the url of the search engine, like "https://www.google.com" or "https://www.bing.com"
        query is the query to search for
        num_results is the number of results to return
        
        example for search weather in beijing:
            search("https://www.google.com", "北京天气", 10)
            result is like:
            [{'title': '北京天气预报,北京7天天气预报,北京15天天气预报,北京天气查询', 'url': 'https://www.weather.com.cn/weather/101010100.shtml', 'description': '2 天之前\u2002·\u2002北京
天气预报，及时准确发布中央气象台天气信息，便捷查询北京今日天气，北京周末天气，北京一周天气预报，北京蓝天预报，北京天气预报，北京40日天气预报，还提供北京的生活指数、健康指数、 
交通指数、旅游指数，及时发布北京气象预警信号、各类气象'}, {'title': '北京-天气预报', 'url': 'http://www.nmc.cn/publish/forecast/ABJ/beijing.html', 'description': '5 天之前\u2002·\u2002全球天气公报 全球热带气旋监测公报 WMO第XI海区海事天气公报 国外农业气象监测与作物展望 全球灾害性天气监测月报 全球雨雪落区预报 世界气象中心（北京）门户网 一带一路气象服
务 亚洲沙尘暴预报专业气象中心'}, {'title': '北京 天气预报15天', 'url': 'https://www.tianqi.com/beijing/15/', 'description': '1 天前\u2002·\u2002当前位置：天气网 > 北京天气网> 
北京 天气预报15天 北京 天气预报15天 今日天气：北京市，晴,10℃~20℃,西南风3级，当前温度15℃。'}, {'title': '【北京天气预报】北京天气预报一周,北京天气预报15天,30天 ...', 'url': 'https://www.tianqi.com/beijing/', 'description': '1 天前\u2002·\u2002天气网提供北京天气预报15天,30天,今日天气,明天天气,北京未来一周的天气预报,北京天气,北京实时天气查询,北京天气 
预报一周,7天,10天,40天的天气情况。'}, {'title': '北京天气预报 –北京', 'url': 'http://bj.weather.com.cn/index.shtml', 'description': '2018年5月9日\u2002·\u2002制作维护：中国气 
象局公共气象服务中心、北京市气象局 郑重声明：中国天气网 版权所有 ，未经书面授权禁止使用 京ICP证010385-2号 京公网安备11041400134号 增值电信业务经营许可证B2-20050053'}, {'title': '【北京今天天气预报】北京天气预报24小时详情_北京天气网', 'url': 'https://www.tianqi.com/beijing/today/', 'description': '2024年3月13日\u2002·\u2002北京天气网为您提供北京天气
预报24小时详情、北京今日天气预报，包括今日实时温度、24小时降水概率、湿度、pm2.5、风向、紫外线强度等，助您放心出行。'}, {'title': '北京天气', 'url': 'http://m.nmc.cn/publish/forecast/ABJ/beijing.html', 'description': '5 天之前\u2002·\u2002世界气象中心（北京）门户网 一带一路气象服务 亚洲沙尘暴预报专业气象中心 环境气象 雾预报 霾预报 沙尘天气 预报 空气
污染气象条件预报 环境气象公报 大气环境气象公报 农业气象 土壤水分监测 农业干旱综合监测 关键农时农事 农业气象周报 农业 ...'}, {'title': '北京今天晴转多云最高温20℃ 清明假期天气干
燥多风 - 腾讯网', 'url': 'https://news.qq.com/rain/a/20250403A01QS200', 'description': '1 天前\u2002·\u2002今天，北京天气晴转多云，气温变化不大。北京市气象台今晨6时发布天气预 
报，预计，今天白天晴转多云，南风三级左右，阵风六级，最高气温20 ...'}, {'title': '【北京天气】北京40天天气预报,北京更长预报,北京天气日历 ...', 'url': 'https://www.weather.com.cn/weather40d/101010100.shtml', 'description': '2 天之前\u2002·\u2002北京天气预报，及时准确发布中央气象台天气信息，便捷查询北京今日天气，北京周末天气，北京一周天气预报，北京15 
日天气预报，,北京40日天气预报，北京天气预报还提供北京各区县的生活指数、健康指数、交通指数、旅游指数，及时发布北京气象预警信号'}, {'title': '【北京天气预报40天】_北京未来40天天
气预报查询-2345 ...', 'url': 'https://tianqi.2345.com/wea_forty/54511.htm', 'description': '4 天之前\u2002·\u20022345天气预报提供北京天气预报40天，北京未来40天天气预报查询服务
，实时更新北京未来一个月天气，精准天气查询就选2345天气预报！'}]
        '''
        from ...Web.Core import web_search
        if url is None:
            url = "https://www.bing.com"
        if query is None:
            if "input" in kwargs:
                query = kwargs["input"]
            else:
                raise ValueError("query is required")
        return await web_search(url, query, num_results)

    @staticmethod
    def get_page_html(url:str, **kwargs) -> str:
        '''
        use the {url} to get the html of the webpage

        url is the url of the webpage to get the html of
        '''
        from ...Web.Core import get_webpage_html
        return get_webpage_html(url)

    @staticmethod
    async def get_page_text(url:str, **kwargs) -> str:
        '''
        use the {url} to get the text of the webpage

        url is the url of the webpage to get the text of
        
        example for get first url when search "北京天气":
            get_page_text("https://www.weather.com.cn/weather/101010100.shtml")
            result is like:4日（今天） 多云转晴 22 / 7℃ 3-4级转<3级 5日（明天） 晴 22 / 8℃ <3级 6日（后天） 晴 24 / 8℃ <3级 7日（周一） 多云 25 / 10℃ <3级 8日（周二） 多云 23 / 12℃ <3级 9日（周三） 阴转多云 21 / 11℃ <3级 10日（周四） 多云转晴 24 / 11℃ <3级 分时 
段预报 生活指数 蓝天预报 蓝天预报综合天气现象、能见度、空气质量等因子，预测未来一周的天空状况。 天空蔚蓝 可见透彻蓝天，或有蓝天白云美景。......
        '''
        from ...Web.Core import get_webpage_text
        return get_webpage_text(url)

    @staticmethod
    async def deep_search(
        url:            Optional[str] = None, 
        query:          Optional[str] = None, 
        num_results:    int = 10, 
        **kwargs) -> List[str]:
        '''
        use the {url} to search the first {num_results} result on the web for the query {query},
        it is a deep search, so it will search the result from the {url} and then search the result from the result

        url is the url of the search engine, like "https://www.google.com" or "https://www.bing.com"
        query is the query to search for
        num_results is the webpage of results to return
        
        using search function to get the urls, and then using get_page_text function to get the text of the webpage
        '''
        from ...Web.Core import web_search
        if url is None:
            url = "https://www.bing.com"
        if query is None:
            if "input" in kwargs:
                query = kwargs["input"]
            else:
                raise ValueError("query is required")
        urls = [result['url'] for result in await web_search(url, query, num_results)]
        texts = []
        for url in urls:
            text = await WebTool.get_page_text(url)
            texts.append(text)
        return texts

    @staticmethod
    def build_tool() -> List[BaseTool]:
        all_function = [
            make_async_func_tool(WebTool.search),
            make_sync_func_tool(WebTool.get_page_text),
            make_async_func_tool(WebTool.deep_search),
            make_sync_func_tool(WebTool.get_page_html)
            ]
        return all_function

class BingTool:
    @staticmethod
    async def search(
        query:          Optional[str] = None, 
        num_results:    int = 10, 
        **kwargs) -> str:
        '''
        use the bing to search the first {num_results} result on the web for the query {query}
        
        is using the bing to search the result, like WebTool.search
        '''
        from ...Web.Core import web_search
        if query is None:
            if "input" in kwargs:
                query = kwargs["input"]
            else:
                raise ValueError("query is required")
        return await web_search("https://www.bing.com", query, num_results)
    
    @staticmethod
    async def bing_search(
        query:          Optional[str] = None, 
        num_results:    int = 10, 
        **kwargs) -> str:
        '''
        use the bing to search the first {num_results} result on the web for the query {query}
        
        is using the bing to search the result, like WebTool.search
        '''
        from ...Web.Core import web_search
        if query is None:
            if "input" in kwargs:
                query = kwargs["input"]
            else:
                raise ValueError("query is required")
        return await web_search("https://www.bing.com", query, num_results)
    
    @staticmethod
    def get_page_html(url:str, **kwargs) -> str:
        '''
        use the {url} to get the html of the webpage

        url is the url of the webpage to get the html of
        
        '''
        from ...Web.Core import get_webpage_html
        return get_webpage_html(url)
    
    @staticmethod
    async def get_page_text(url:str, **kwargs) -> str:
        '''
        use the {url} to get the text of the webpage

        url is the url of the webpage to get the text of
        
        is using the bing to search the result, like WebTool.get_page_text
        '''
        from ...Web.Core import get_webpage_text
        return get_webpage_text(url)
    
    @staticmethod
    async def deep_search(
        query:          Optional[str] = None, 
        num_results:    int = 10, 
        **kwargs) -> List[str]:
        '''
        use the bing to search the first {num_results} result on the web for the query {query},
        it is a deep search, so it will search the result from the bing and then search the result from the result
        
        is using the bing to search the result, like WebTool.deep_search
        '''
        if query is None:
            if "input" in kwargs:
                query = kwargs["input"]
            else:
                raise ValueError("query is required")
        return await WebTool.deep_search("https://www.bing.com", query, num_results)
    
    
    @staticmethod
    def build_tool() -> List[BaseTool]:
        all_function = [
            make_async_func_tool(BingTool.search),
            make_async_func_tool(BingTool.bing_search),
            make_sync_func_tool(BingTool.get_page_text),
            make_async_func_tool(BingTool.deep_search),
            make_sync_func_tool(BingTool.get_page_html)
            ]
        return all_function

WebTools = WebTool.build_tool()
BingTools = BingTool.build_tool()

__all__ = [
    'WebTools',
    'BingTools'
]