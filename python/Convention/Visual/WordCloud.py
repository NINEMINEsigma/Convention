from ..Internal import *
from pyecharts.charts import WordCloud
from pyecharts import options as opts
from pyecharts import types
from ..File.Core import tool_file, UnWrapper as UnWrapper2Str

def make_word_cloud(
        series_name:    str,
        data_pair:      Sequence[Tuple[str, int]],
        **kwargs,
    ):
    wordcloud = WordCloud()
    wordcloud.add(series_name, data_pair, **kwargs)
    return wordcloud

def set_title(
    wordcloud:          WordCloud,
    title:              str
):
    wordcloud.set_global_opts(
        title_opts=opts.TitleOpts(title=title)
    )

def render_to(
    wordcloud:          WordCloud,
    file_name:          Union[tool_file, str]
):
    wordcloud.render(UnWrapper2Str(file_name))

class light_word_cloud(left_value_reference[WordCloud]):
    def __init__(
        self,
        series_name:    str,
        data_pair:      types.Sequence,
        **kwargs,
    ):
        super().__init__(make_word_cloud(series_name, data_pair, **kwargs))

    def set_title(
        self,
        title:          str
    ):
        set_title(self.ref_value, title)

    def render_to(
        self,
        file_name:      Union[tool_file, str]
    ):
        render_to(self.ref_value, file_name)

if __name__ == "__main__":
    # 准备数据
    wordcloud = make_word_cloud("", [
        ("Python", 100),
        ("Java", 80),
        ("C++", 70),
        ("JavaScript", 90),
        ("Go", 60),
        ("Rust", 50),
        ("C#", 40),
        ("PHP", 30),
        ("Swift", 20),
        ("Kotlin", 10),
    ], word_size_range=[20, 100])
    set_title(wordcloud, "cloud")
    render_to(wordcloud, "wordcloud.html")
