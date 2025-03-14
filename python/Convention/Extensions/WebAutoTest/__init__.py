# -*- coding: utf-8 -*-
from ...Web.Selunit import *
from ...lazy import *
from ...Lang.CppLike import *
from ...Lang.Reflection import get_type_from_string

def __internal_build_up_ex() -> selunit:
    config = ProjectConfig()
    # browser type init
    browser_type:   str         =  config.FindItem("browser_type")
    browser:        WebDriver   = None
    if browser_type is not None:
        if browser_type == "Chrome":
            browser = base_webdriver.Chrome()
        elif browser_type == "Firefox":
            browser = base_webdriver.Firefox()
        elif browser_type == "Edge":
            browser = base_webdriver.Edge()
        elif browser_type == "Safari":
            browser = base_webdriver.Safari()
        else:
            browser = get_type_from_string(browser_type)
    else:
        config["browser_type"] = "Edge"
    # delay init
    delay:          float       = config.FindItem("delay")
    if delay is None:
        delay = 3
        config["delay"] = 3
    # config init
    result = selunit(browser, delay=delay)
    for key in config:
        # url init
        if "url" in key:
            if "begin" in key or "start" in key:
                result.open(config[key])
            else:
                result.open(config[key])
    # implicitly_wait init
    implicitly_wait:bool        = config.FindItem("implicitly_wait")
    if implicitly_wait is None:
        implicitly_wait = False
        config["implicitly_wait"] = False
    if implicitly_wait != False and implicitly_wait != 0:
        result.set_implicitly_wait_mode()

    config.save_properties()

    return result

set_buildup_static_selunit_instance_func(__internal_build_up_ex)

# classname_tool_any 作为可直接载入的Action
# classname_tool_do_any 可以生成能够载入的Action

def login_tool_username_password_login(browser:selunit):
    config = ProjectConfig()
    if "not_auto_login" in config:
        return None
    username = config.FindItem("username")
    password = config.FindItem("password")
    if username is None or password is None:
        return None
    browser.find_username()[0].send_keys(str(username))
    browser.find_password()[0].send_keys(str(password))
    browser.find_login_element()[0].ref_value.click()

class login_page(page):
    def __init__(
        self,
        *actions:   Action[selunit]
        ):
        super().__init__(
            login_tool_username_password_login,
            *remove_none_value(actions),
            )

def process_tool_do_button_path(bytypen:ByTypen, *texts:str, is_throw=True) -> Action[selunit]:
    def closure_process_tool(browser:selunit):
        for text in texts:
            element = browser.find_element(text, bytypen)
            if element is not None:
                element.click()
            elif is_throw:
                raise ValueError(f"Can't find button with find_element and config is: {make_dict(
                    by=bytypen,
                    text=text,
                    texts=texts,
                )}")
    return closure_process_tool
def process_tool_easy_button_path(*texts:str) -> Action[selunit]:
    def closure_process_tool(browser:selunit):
        for text in texts:
            elements = browser.find_anylike_with_xpath(text)
            if len(elements) != 0:
                elements[0].click()
            else:
                raise ValueError(f"Can't find button with find_element and config is: {make_dict(
                    text=text,
                    texts=texts,
                )}")
    return closure_process_tool

class process_page(page):
    def __init__(
        self,
        *actions:   Action[selunit]
        ):
        super().__init__(
            *actions
            )

class action_page(page):
    def __init__(
        self,
        *actions:   Action[selunit]
        ):
        super().__init__(
            *remove_none_value(actions)
            )





