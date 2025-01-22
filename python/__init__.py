from Convention.Internal import *

class test(type_class):
    pass

if __name__ == "__main__":
    print(test)
    print(test())
    print(test().GetType())
    print(test().ToString())
    print(test().SymbolName())