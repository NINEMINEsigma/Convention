from Convention.Internal import __DEBUG__

__DEBUG__ = True

from Convention.LLM.LlamaIndex.Core import *

def run():
    print("test_embinding")
    embedding = EmbeddingCore(model_uid="", base_url="http://127.0.0.1:61111")
    embedding.set_as_global_embedding()

    print(embedding.get_general_text_embedding("你好"))

if __name__ == "__main__":
    run()

