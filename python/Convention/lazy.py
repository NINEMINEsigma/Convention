from .Internal     import *
from .Str.Core     import UnWrapper as Unwrapper2Str, fill_str
from .File.Core    import tool_file, tool_file_or_str, Wrapper as Wrapper2File
import os

const_config_file = "config.json"

def generate_empty_config_json(path:tool_file_or_str):
    file = Wrapper2File(path)
    file.open('w')
    file.data = {"properties":{}}
    file.save()
    return file

class GlobalConfig(any_class):
    configLogging_tspace:   int         = len("Property not found")

    def get_config_file(self):
        return self.data_dir|self.__const_config_file

    @property
    def config_file(self):
        return self.get_config_file()

    def __init__(
            self,
            data_dir:                   Optional[tool_file_or_str]  = None,
            is_try_create_data_dir:     bool                        = False,
            load:                       bool                        = True,
            ):
        # build up data folder
        if data_dir is None:
            data_dir = tool_file(os.path.abspath('.'))
        self.data_dir:tool_file = data_dir if isinstance(data_dir, tool_file) else tool_file(Unwrapper2Str(data_dir))
        if self.data_dir.is_dir() is False:
            self.data_dir.back_to_parent_dir()
        if self.data_dir.exists() is False:
            if is_try_create_data_dir:
                self.data_dir.must_exists_path()
            else:
                raise FileNotFoundError(f"Can't find data dir: {self.data_dir.get_dir()}")
        # build up init data file
        self.__data_pair:Dict[str, Any] = {}
        global const_config_file
        self.__const_config_file = const_config_file
        config_file = self.config_file
        if config_file.exists() is False:
            generate_empty_config_json(config_file)
        elif load:
            self.load_properties()
    def __del__(self):
        #self.save_properties()
        pass

    def save_temp_data(self, any_data):
        self.data_dir.data = any_data
        self.data_dir.save()
    def load_temp_data(self):
        return self.data_dir.load()

    def get_file(self, file:str, is_must:bool=False):
        result = self.data_dir|file
        if is_must and result.exists() is False:
            result.must_exists_path()
        return result
    def erase_file(self, file:str):
        result = self.data_dir|file
        if result.exists():
            try:
                with open(result.get_path(), "wb") as _:
                    return True
            except:
                pass
        return False
    def remove_file(self, file:str):
        result = self.data_dir|file
        if result.exists():
            try:
                result.remove()
                return True
            except:
                pass
        return False
    def create_file(self, file:str):
        result = self.data_dir|file
        if result.exists():
            return False
        if result.back_to_parent_dir().exists() is False:
            return False
        result.create()
        return True

    def __setitem__(self, key:str, value:str):
        self.__data_pair[key] = value
    def __getitem__(self, key:str):
        return self.__data_pair[key]
    def __contains__(self, key:str):
        return key in self.__data_pair
    def __delitem__(self, key:str):
        del self.__data_pair[key]
    def __iter__(self):
        return iter(self.__data_pair)
    def __len__(self):
        return len(self.__data_pair)

    def save_properties(self):
        config = self.config_file
        config.open('w', encoding='utf-8')
        config.data = {
            "properties": self.__data_pair
        }
        config.save()
        return self
    def __internal_load_properties(self):
        config = self.config_file
        if config.exists() is False:
            self.__data_pair = {}
        else:
            config.load()
            if "properties" in config.data:
                for property_name in config.data["properties"]:
                    self.__data_pair[property_name] = config.data["properties"][property_name]
            else:
                raise ValueError("Can't find properties in config file")
        return self
    def load_properties(self):
        self.__internal_load_properties()
        return self

    def print_source_pair(self):
        print(self.__data_pair)

    def get_log_file(self) -> tool_file:
        return self.get_file(self.config_file.get_filename(True)+"_log.txt", True)
    @property
    def log_file(self) -> tool_file:
        return self.get_log_file()
    @virtual
    def Log(self, message_type:str, message:Union[str, Any], color:str):
        str_message_type = str(message_type)
        self.configLogging_tspace = max(self.configLogging_tspace, len(str_message_type))
        what = f"{fill_str(message_type, self.configLogging_tspace, side="center")}: {Unwrapper2Str(message)}"
        print_colorful(color,what,is_reset=True)
        log = self.get_log_file()
        log.open('a')
        log.write(f"[{nowf()}]{what}\n")
        return self
    def LogMessage(self, message:str, color:str=ConsoleFrontColor.BLUE):
        self.Log("Message", message, color)
        return self
    def LogWarning(self, message:str):
        self.Log("Warning", message, ConsoleFrontColor.YELLOW)
        return self
    def LogError(self, message:str):
        self.Log("Error", message, ConsoleFrontColor.RED)
        return self
    def LogPropertyNotFound(self, message, default=None):
        if default is None:
            self.Log("Property not found", message, ConsoleFrontColor.YELLOW)
        else:
            self.Log("Property not found", f"{message}(default = {default})", ConsoleFrontColor.YELLOW)
        return self
    def LogMessageOfPleaseCompleteConfiguration(self):
        self.LogError("Please complete configuration")
        return self

    def LogClear(self):
        self.get_log_file().remove()
        return self

    def FindItem(self, key:str, default=None):
        if key in self.__data_pair:
            return self.__data_pair[key]
        else:
            self.LogPropertyNotFound(key, default)
            return default

_ProjectConfig_FileFocus:tool_file_or_str = "Assets/"

class ProjectConfig(GlobalConfig):
    def __init__(self, load=True):
        super().__init__(ProjectConfig.GetProjectAssets(), is_try_create_data_dir=True, load=load)
        
    @staticmethod
    def SetProjectAssets(path:tool_file_or_str):
        global _ProjectConfig_FileFocus
        _ProjectConfig_FileFocus = path
    
    @staticmethod
    def GetProjectAssets():
        global _ProjectConfig_FileFocus
        return _ProjectConfig_FileFocus
    

if __name__ == "__main__":
    pass








