from Convention.Internal import *

INTERNAL_DEBUG = True

from Convention.LLM.LlamaIndex.Core import *

def run():
    print("test_embinding")
    embedding = CustomEmbedding(
        model_uid="",
        url="http://10.10.230.60:61111"
        #url="http://127.0.0.1:61111"
        )
    embedding.set_as_global_embedding()

    print(embedding.get_general_text_embedding("你好"))

if __name__ == "__main__":
    run()

