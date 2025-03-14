from typing                         import *
from ..Internal                 import *
import numpy                        as     np
import cv2                          as     cv2
from cv2.typing                     import *
import scipy                        as     sci_base
from scipy          import optimize as     sci_opt

BasicIntFloatNumber     = Union[int, float]
NpSignedIntNumber       = Union[
    np.int8,
    np.int16,
    np.int32,
    np.int64,
]
NpUnsignedIntNumber     = Union[
    np.uint8,
    np.uint16,
    np.uint32,
    np.uint64,
]
NpFloatNumber           = Union[
    np.float16,
    np.float32,
    np.float64,
]
NumberTypeOf_Float_Int_And_More = Union[
    BasicIntFloatNumber,
    NpSignedIntNumber,
    NpUnsignedIntNumber,
    NpFloatNumber,
    np.ndarray
]
NumberLike              = NumberTypeOf_Float_Int_And_More
class left_number_reference[_T:NumberLike](left_value_reference[_T]):
    def __init__(self, ref_value:NumberLike):
        super().__init__(ref_value)
    def is_array(self):
        return isinstance(self.ref_value, np.ndarray)
    def is_scalar(self):
        return not self.is_array()
    @property
    def ndim(self):
        if self.is_empty():
            return -1
        elif self.is_array():
            return self.ref_value.ndim
        else:
            return 0
    @property
    def shape(self):
        if self.is_empty():
            return (-1)
        elif self.is_array():
            return self.ref_value.shape
        else:
            return (0)

    NumberLike_or_Self = Union[NumberLike, Self]

    def __add__(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return np.add(self.ref_value, value)
    def __sub__(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return np.subtract(self.ref_value, value)
    def __mul__(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return np.multiply(self.ref_value, value)
    def __truediv__(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return np.true_divide(self.ref_value, value)
    def __floordiv__(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return np.floor_divide(self.ref_value, value)
    def __mod__(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return np.mod(self.ref_value, value)
    def __pow__(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return np.pow(self.ref_value, value)
    def __radd__(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return np.add(value, self.ref_value)
    def __rsub__(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return np.subtract(value, self.ref_value)
    def __rmul__(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return np.multiply(value, self.ref_value)
    def __rtruediv__(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return np.true_divide(value, self.ref_value)
    def __rfloordiv__(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return np.floor_divide(value, self.ref_value)
    def __rmod__(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return np.mod(value, self.ref_value)
    def __rpow__(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return np.pow(value, self.ref_value)
    def __neg__(self):
        return np.negative(self.ref_value)
    def __pos__(self):
        return np.positive(self.ref_value)
    def __abs__(self):
        return np.abs(self.ref_value)
    def __round__(self, ndigits:Optional[int]=None):
        return np.round(self.ref_value, ndigits)
    def __eq__(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return np.equal(self.ref_value, value)
    def __ne__(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return self.__eq__(value) is False
    def __lt__(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return self.ref_value < value
    def __le__(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return self.ref_value <= value
    def __gt__(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return self.ref_value > value
    def __ge__(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return self.ref_value >= value
    def __bool__(self):
        if self.is_empty():
            return False
        return self.ref_value != 0
    def __hash__(self):
        return hash(self.ref_value)
    def __repr__(self):
        if self.is_empty():
            return "None"
        return repr(self.ref_value)
    def __str__(self):
        return str(self.ref_value)
    def __neg__(self):
        return np.negative(self.ref_value)
    def bitwise_and(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return np.bitwise_and(self.ref_value, value)
    def bitwise_or(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return np.bitwise_or(self.ref_value, value)
    def bitwise_xor(self, value:NumberLike_or_Self):
        if isinstance(value, left_number_reference):
            value = value.ref_value
        return np.bitwise_xor(self.ref_value, value)
    def bitwise_not(self):
        return np.bitwise_not(self.ref_value)
def Wrapper2Lvn(value:Union[
    NumberLike,
    base_value_reference,
    ]) -> left_number_reference:
    if isinstance(value, left_number_reference):
        return value
    target = value
    if isinstance(target, (left_value_reference, right_value_refenence)):
        target = target.ref_value
    return left_number_reference(target)
class left_np_ndarray_reference(left_number_reference[np.ndarray]):
    '''
    针对np.ndarray的特化引用:
        将np.ndarray操作封装为面向对象的形式
    '''
    def __init__(self, ref_value:Optional[np.ndarray]=None):
        super().__init__(ref_value)
    def where(
        self,
        pr:Union[Callable, Any]
        ) -> np.ndarray:
        '''
        get a mask by pr
        '''
        if isinstance(pr, Callable):
            return np.where(pr(self.ref_value))
        else:
            return np.where(pr==self.ref_value)
    def clip(
        self,
        mini,
        maxi
        ) -> np.ndarray:
        return np.clip(self.ref_value, mini, maxi)

    def __getitem__(self, *args, **kwargs) -> np.ndarray:
        return self.ref_value.__getitem__(*args, **kwargs)
    def __setitem__(self, *args, **kwargs):
        return self.ref_value.__setitem__(*args, **kwargs)
    def astype(self, typen) -> np.ndarray:
        return self.ref_value.astype(typen)
    @property
    def element_type(self):
        return self.ref_value.dtype

    def __array__(self) -> np.ndarray:
        return self.ref_value
    def get_array(self) -> np.ndarray:
        return self.ref_value
    # 序列合并
    def _inject_stack_uniform_item(self):
        return self.get_array()
    def stack(self, *args:Self, **kwargs) -> np.ndarray:
        container = [self]
        container.extend([item for item in args])
        return np.stack([
            item._inject_stack_uniform_item() for item in container
            ], *args, **kwargs)
    def vstack(self, *args:Self) -> np.ndarray:
        container = [self]
        container.extend([item for item in args])
        return np.vstack([item._inject_stack_uniform_item() for item in container])
    def hstack(self, *args:Self) -> np.ndarray:
        container = [self]
        container.extend([items for items in args])
        return np.hstack([item._inject_stack_uniform_item() for item in container])
    # 标准化与规范化
    def normalize(self, mini, maxi) -> np.ndarray:
        return (self.ref_value-mini)/(maxi- mini)*255
    def standardize(self, mean, std) -> np.ndarray:
        return (self.ref_value-mean)/std
    # np加速
    def item(self, *args:SupportsIndex):
        return self.ref_value.item(*args)
    def item_set(self, value, *args:SupportsIndex):
        self.ref_value.itemset(args, value)
    # np内容
    @property
    def shape(self) -> Tuple[int, ...]:
        return self.ref_value.shape
    @property
    def element_count(self) -> int:
        return self.ref_value.size
    @property
    def dimension(self) -> int:
        return self.ref_value.ndim

def Wrapper2Lvnp(
    value:  Union[
        NumberLike,
        base_value_reference,
        ]
    ) -> left_np_ndarray_reference:
    if isinstance(value, left_np_ndarray_reference):
        return value
    target = value
    if isinstance(target, (left_value_reference, right_value_refenence)):
        target = target.ref_value
    if isinstance(target, np.ndarray) is False:
        target = np.array(target)
    return left_np_ndarray_reference(target)

NumberInside = NumberLike
NumberLike_or_lvNumber = Union[
    NumberLike,
    left_number_reference,
]

def UnwrapperLvn2Number(value:NumberLike_or_lvNumber):
    if isinstance(value, left_number_reference):
        return value.ref_value
    return value

def internal_max(*args):
    if isinstance(args[0], (Sequence,Generator)):
        return max(args[0])
    result = args[0]
    for item in args:
        if result<item:
            result = item
    return result
def internal_min(*args):
    if isinstance(args[0], (Sequence,Generator)):
        return min(args[0])
    result = args[0]
    for item in args:
        if item<result:
            result = item
    return result

def clamp_without_check(
    value:  NumberLike_or_lvNumber,
    left:   NumberLike_or_lvNumber,
    right:  NumberLike_or_lvNumber
    ) -> NumberLike:
    return type(value)(internal_max(left, internal_min(value, right)))
def clamp(
    value:  NumberLike_or_lvNumber,
    a:      NumberLike_or_lvNumber,
    b:      NumberLike_or_lvNumber
    ) -> NumberLike:
    if hasattr(value, "__lt__") is False:
        return value
    if a>b:
        c = a
        a = b
        b = c
    return clamp_without_check(value, b, a)
def clamp_sequence(
    values: Sequence[NumberLike_or_lvNumber],
    a:      NumberLike_or_lvNumber,
    b:      NumberLike_or_lvNumber
    ) -> Sequence[NumberLike]:
    if a>b:
        c = a
        a = b
        b = c
    return type(values)([clamp_without_check(value, a, b) for value in values])
def clamp_dict(
    values: Dict[Any, Union[
        NumberLike,
        left_number_reference
    ]],
    a:      NumberLike,
    b:      NumberLike
    ) -> Dict[Any, NumberLike]:
    if a>b:
        c = a
        a = b
        b = c
    return {key: clamp_without_check(value, b, a) for key, value in values.items()}
def make_clamp(
    value_or_values:    Union[
        NumberLike,
        Sequence[NumberLike],
        Dict[Any, NumberLike]
    ],
    a:                  NumberLike,
    b:                  NumberLike
    ) -> Union[NumberLike, Sequence[NumberLike], Dict[str, NumberLike]]:
    if isinstance(value_or_values, NumberLike):
        return clamp(value_or_values, a, b)
    elif isinstance(value_or_values, Sequence):
        return clamp_sequence(value_or_values, a, b)
    elif isinstance(value_or_values, dict):
        return clamp_dict(value_or_values, a, b)
    else:
        raise TypeError("value_or_values must be NumberLike, Sequence or Dict")

NumberBetween01 = Union[float, NpFloatNumber]
FloatBetween01 = float
def clamp01[_T](value:_T) -> _T:
    return make_clamp(value, 0, 1)







EquationsLike = Callable[[Sequence[float]], Sequence[float]]
class EquationsCallable(EquationsLike):
    def __init__(self, equations:EquationsLike, args_size):
        self.__equations = equations
        self.param_size = args_size
    def __call__(self, args:Sequence[float]) -> Sequence[float]:
        return self.__equations(args)

def make_equations(
    equations:Sequence[Callable[[float], float]]
    ) -> EquationsCallable:
    def equations_solution(args:Sequence[float]) -> Sequence[float]:
        result = []
        for index, equation in enumerate(equations):
            result.append(equation(args[index]))
        return result
    # Closer
    return EquationsCallable(equations_solution, len(equations))

class solver:
    def __init__(self, **kwargs):
        self.__config = kwargs

    def opt_fsolve(
        self,
        equations:      EquationsLike,
        initer:         Optional[Union[Sequence[float], float]] = None,
        **kwargs
        ) -> Sequence[float]:
        if initer is None:
            equations:EquationsCallable = equations
            initer = [0.001 for _ in range(equations.param_size)]
        elif isinstance(initer, float):
            equations:EquationsCallable = equations
            initer = [initer for _ in range(equations.param_size)]
        return sci_opt.fsolve(equations, initer, **kwargs)


if __name__ == "__main__":
    solver_i = solver()
    equs = make_equations([lambda x: x**2 - 1])
    print(solver_i.opt_fsolve(equs))
