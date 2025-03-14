from ..Internal import *
from ..File.Core import *
from bs4 import BeautifulSoup as base
import bs4

class light_bs(left_value_reference[base]):
    @override
    def ToString(self):
        return f"bs<{self.__base}>"

    def __init__(
        self,
        markup:             Union[
            str, bytes, "bs4.SupportsRead[str]", "bs4.SupportsRead[bytes]",
            tool_file,
        ]                                       = "",
        features:           Optional[Union[
            str,
            Sequence[str]
        ]]                                      = "html.parser",
        builder:            Optional[Union[
            "bs4.TreeBuilder",
            type["bs4.TreeBuilder"]
        ]]                                      = None,
        parse_only:         Optional[
            bs4.SoupStrainer
        ]                                       = None,
        from_encoding:      Optional[str]       = None,
        exclude_encodings:  Optional[
            Sequence[str]
        ]                                       = None,
        element_classes:    Optional[Dict[
            type["bs4.PageElement"], type[Any]
        ]]                                      = None,
        **kwargs
        ):
        if is_loss_tool_file(markup):
            super().__init__(None)
        origin_markup = markup
        if isinstance(markup, tool_file):
            origin_markup = markup.load()
        super().__init__(base(origin_markup,
            features, builder, parse_only, from_encoding,
            exclude_encodings, element_classes, **kwargs))
    @property
    def __base(self):
        return self.ref_value

    def first(self, tag, attrs=None, recursive=True, text=None, **kwargs):
        return self.__base.find(tag, attrs, recursive, text, **kwargs)

    def find(self, tag, attrs=None, recursive=True, text=None, **kwargs):
        return self.__base.find(tag, attrs, recursive, text, **kwargs)

    def find_all(self, tag, attrs=None, recursive=True, text=None, limit=None, **kwargs):
        return self.__base.find_all(tag, attrs, recursive, text, limit, **kwargs)

    def select(self, selector):
        return self.__base.select(selector)

    @property
    def text(self):
        return self.__base.text

if __name__ == "__main__":
    core = light_bs()
