from typing             import *
import requests         as     base
from requests.adapters  import HTTPAdapter
import                         urllib3

class light_requests(object):
    def __init__(
        self,
        retries:            int                     = 3,
        backoff_factor:     int                     = 0.3,
        status_forcelist:   tuple                   = (500, 502, 504)
        ):
        self.session = base.Session()
        retry_strategy = urllib3.Retry(
            total=retries,
            status_forcelist=status_forcelist,
            allowed_methods=["HEAD", "GET", "OPTIONS"],
            backoff_factor=backoff_factor,
        )
        adapter = HTTPAdapter(max_retries=retry_strategy)
        self.session.mount("http://", adapter)
        self.session.mount("https://", adapter)
    
    def get(
        self,
        url:        str,
        timeout:    int = 30,
        headers         = None, 
        **kwargs
        ):
        return self.session.get(url, timeout=timeout, headers=headers, **kwargs)
    
    def post_data(
        self,
        url:        str,
        data:       Optional[Any] = None,
        timeout:    int           = 30, 
        **kwargs
        ):
        return self.session.post(url, data=data, timeout=timeout, **kwargs)
    
    def post_json(
        self, 
        url:        str,
        json:       Optional[Any] = None, 
        timeout:    int           = 30,
        **kwargs
        ):
        return self.session.post(url, json=json, timeout=timeout, **kwargs)
    
    def put(
        self, 
        url:        str,
        data:       Optional[Any] = None,
        timeout:    int           =30,
        **kwargs
        ):
        return self.session.put(url, data=data, timeout=timeout, **kwargs)
    
    def delete(
        self, 
        url:        str,
        timeout:    int = 30,
        **kwargs
        ):
        return self.session.delete(url, timeout=timeout, **kwargs)
    

if __name__ == "__main__":
    # Example usage
    light = light_requests()
    url = r"https://www.baidu.com"
    
    response = light.get(url)
    print(response.status_code)
    print(response.text[:200])
    
    