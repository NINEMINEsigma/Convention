from Convention.Internal import *

class instance[_T](ref[_T], any_class):
    def __init__(self, value:_T = None):
        return super().__init__(value)
    def is_empty(self):
        return self.value is None
    @override
    def ToString(self):
        return str(addressof(self.value))
    