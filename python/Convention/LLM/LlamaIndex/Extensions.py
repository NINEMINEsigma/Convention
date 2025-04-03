from .Core                  import *

class WebTool:
    @staticmethod
    async def search(url:str, query:str, num_results:int = 10, **kwargs) -> str:
        '''
        use the {url} to search the first {num_results} result on the web for the query {query}

        url is the url of the search engine, like "https://www.google.com" or "https://www.bing.com"
        query is the query to search for
        num_results is the number of results to return
        '''
        from ...Web.Core import web_search
        return await web_search(url, query, num_results)

    @staticmethod
    async def bing_search(query:str, num_results:int = 10, **kwargs) -> str:
        '''
        use the bing to search the first {num_results} result on the web for the query {query}

        query is the query to search for
        num_results is the number of results to return
        '''
        from ...Web.Core import web_search
        return await web_search("https://www.bing.com", query, num_results)

    @staticmethod
    async def get_page_text(url:str, **kwargs) -> str:
        '''
        use the {url} to get the text of the webpage

        url is the url of the webpage to get the text of
        '''
        from ...Web.Core import get_webpage_text
        return get_webpage_text(url)

    @staticmethod
    async def deep_search(url:str, query:str, num_results:int = 10, **kwargs) -> List[str]:
        '''
        use the {url} to search the first {num_results} result on the web for the query {query},
        it is a deep search, so it will search the result from the {url} and then search the result from the result

        url is the url of the search engine, like "https://www.google.com" or "https://www.bing.com"
        query is the query to search for
        num_results is the webpage of results to return
        '''
        from ...Web.Core import web_search
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
            make_async_func_tool(WebTool.deep_search)
            ]
        return all_function

WebTools = WebTool.build_tool()

__all__ = [
    'WebTools',
]