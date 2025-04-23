import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from Convention.Lang.Core import *
from Convention.Lang.Reflection import *
from Convention.Workflow import *
from Convention.LLM.LlamaIndex.Core import *

def run():
    LLMObject.using_LlamaCPP_from_url("http://10.10.230.60:61111").set_as_global_llm()
    CustomEmbedding("http://10.10.230.60:61111/embeddings", 120).set_as_global_embedding()
    index = IndexCore(VectorStoreIndex.from_documents(SimpleDirectoryReader(input_files=[r"./tests/global_knowledge_base/天气.txt"]).load_data()))
    print(index.query("今天天气怎么样"))

if __name__ == "__main__":
    run()