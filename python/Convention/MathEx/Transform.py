from typing             import *
from ..Internal     import *
from abc                import *
import math             as     base
from ..MathEx.Core  import *

# region Boxing with no angle

class abs_box(any_class, ABC):
    @override
    def ToString(self):
        return str(self)
    @abstractmethod
    def __str__(self) -> str:
        raise NotImplementedError()
    @abstractmethod
    def __repr__(self) -> str:
        raise NotImplementedError()
    def __eq__(self, other:Self) -> bool:
        return (
            self.get_left_edge() == other.get_left_edge() and
            self.get_right_edge() == other.get_right_edge() and
            self.get_top_edge() == other.get_top_edge() and
            self.get_bottom_edge() == other.get_bottom_edge()
            )
    def __ne__(self, other:Self) -> bool:
        return not self.__eq__(other)
    @abstractmethod
    def to_json(self) -> Union[Dict[str, NumberLike], str]:
        raise NotImplementedError()
    @abstractmethod
    def to_tuple(self) -> Tuple[NumberLike, NumberLike, NumberLike, NumberLike]:
        raise NotImplementedError()
    @abstractmethod
    def to_list(self) -> List[NumberLike]:
        raise NotImplementedError()
    @abstractmethod
    def move(self, dx: NumberLike, dy: NumberLike) -> Self:
        raise NotImplementedError()
    def resize(
        self,
        new_weight:     NumberLike,
        new_height:     NumberLike,
        new_center_x:   Optional[NumberLike] = None,
        new_center_y:   Optional[NumberLike] = None
        ) -> Self:
        old_center_x, old_center_y = self.get_center()
        return self.move(
                new_center_x - old_center_x, new_center_y - old_center_y
            ).scale(
                new_weight/self.get_width(), new_height/self.get_height()
            )
    @abstractmethod
    def scale(self, scale_x: NumberLike, scale_y: NumberLike) -> Self:
        raise NotImplementedError()
    @abstractmethod
    def get_center(self) -> Tuple[NumberLike, NumberLike]:
        raise NotImplementedError()
    def is_square(self) -> bool:
        width = self.get_width()
        height = self.get_height()
        return not base.isclose(0) and not base.isclose(height) and base.isclose(width, height)
    def is_line(self) -> bool:
        return base.isclose(self.get_width(), 0) != base.isclose(self.get_height(), 0)
    def is_point(self) -> bool:
        return base.isclose(self.get_width(), 0) and base.isclose(self.get_height(), 0)
    def get_width(self) -> NumberLike:
        return self.get_right_edge() - self.get_left_edge()
    def get_height(self) -> NumberLike:
        return self.get_top_edge() - self.get_bottom_edge()

    @property
    def area(self) -> NumberLike:
        return self.get_width()*self.get_height()
    @property
    def center(self) -> Tuple[NumberLike, NumberLike]:
        return self.get_center()
    @property
    def aspect_ratio(self) -> NumberLike:
        return self.get_width()/self.get_height()
    @property
    def diagonal(self):
        return (self.get_width() ** 2 + self.get_height() ** 2) ** 0.5

    @abstractmethod
    def get_left_edge(self) -> NumberLike:
        raise NotImplementedError()
    @abstractmethod
    def get_right_edge(self) -> NumberLike:
        raise NotImplementedError()
    @abstractmethod
    def get_top_edge(self) -> NumberLike:
        raise NotImplementedError()
    @abstractmethod
    def get_bottom_edge(self) -> NumberLike:
        raise NotImplementedError()

    @abstractmethod
    def make_intersection(self, other:Self) -> Self:
        raise NotImplementedError()
    @abstractmethod
    def make_large_union(self, other:Self) -> Self:
        raise NotImplementedError()
    def __or__(self, other:Self) -> Self:
        return self.make_large_union(other)
    def __and__(self, other:Self) -> Self:
        return self.make_intersection(other)

    def is_collide(self, other:Self):
        return self.make_intersection(other).area > 0
    def is_inside(self, other:Self):
        return base.isclose(self.make_intersection(other).area, self.area)

    def get_lb_pos(self) -> Tuple[NumberLike, NumberLike]:
        return self.get_left_edge(), self.get_bottom_edge()
    def get_lt_pos(self) -> Tuple[NumberLike, NumberLike]:
        return self.get_left_edge(), self.get_top_edge()
    def get_rt_pos(self) -> Tuple[NumberLike, NumberLike]:
        return self.get_right_edge(), self.get_top_edge()
    def get_rb_pos(self) -> Tuple[NumberLike, NumberLike]:
        return self.get_right_edge(), self.get_bottom_edge()

BoxBasicLike = Union[
    Tuple[NumberLike, NumberLike, NumberLike, NumberLike],
    List[NumberLike],
    str,#json {x:..., }
    Dict[str, NumberLike],#json {x:..., }
    np.ndarray
]

class Box(abs_box):
    def __init__(self, left: NumberLike, right: NumberLike, top: NumberLike, bottom: NumberLike):
        self.__inject_array = np.ndarray([left, right, top, bottom])

    @property
    def left(self) -> NumberLike:
        return self.__inject_array[0]
    @left.setter
    def left(self, value: NumberLike):
        self.__inject_array[0] = value
    @property
    def right(self) -> NumberLike:
        return self.__inject_array[1]
    @right.setter
    def right(self, value: NumberLike):
        self.__inject_array[1] = value
    @property
    def top(self) -> NumberLike:
        return self.__inject_array[2]
    @top.setter
    def top(self, value: NumberLike):
        self.__inject_array[2] = value
    @property
    def bottom(self) -> NumberLike:
        return self.__inject_array[3]
    @bottom.setter
    def bottom(self, value: NumberLike):
        self.__inject_array[3] = value

    @property
    def data_array(self):
        return self.__inject_array
    @data_array.setter
    def data_array(self, value: np.ndarray):
        self.__inject_array = value

    @override
    def __str__(self):
        return f"{self.left}, {self.right}, {self.top}, {self.bottom}"
    @override
    def to_json(self) -> Dict[str, NumberLike]:
        return {"left":self.left, "right":self.right, "top":self.top, "bottom":self.bottom}
    @override
    def to_tuple(self):
        return (self.left, self.right, self.top, self.bottom)
    @override
    def to_list(self):
        return [self.left, self.right, self.top, self.bottom]
    @override
    def __repr__(self):
        return f"Box({self.left}, {self.right}, {self.top}, {self.bottom})"

    def __getitem__(self, key):
        if key == 0 or key == "left":
            return self.left
        elif key == 1 or key == "right":
            return self.right
        elif key == 2 or key == "top":
            return self.top
        elif key == 3 or key == "bottom":
            return self.bottom
        else:
            raise IndexError("Index out of range")
    def __setitem__(self, key, value):
        if key == 0 or key == "left":
            self.left = value
        elif key == 1 or key == "right":
            self.right = value
        elif key == 2 or key == "top":
            self.top = value
        elif key == 3 or key == "bottom":
            self.bottom = value
        else:
            raise IndexError("Index out of range")

    @override
    def get_left_edge(self) -> NumberLike:
        return self.left
    @override
    def get_right_edge(self) -> NumberLike:
        return self.right
    @override
    def get_top_edge(self) -> NumberLike:
        return self.top
    @override
    def get_bottom_edge(self) -> NumberLike:
        return self.bottom

    def is_vaild(self):
        return self.left <= self.right and self.top <= self.bottom
    def is_invaild(self):
        return self.left > self.right or self.top > self.bottom

    @override
    def move(self, dx:NumberLike, dy:NumberLike):
        self.left += dx
        self.right += dx
        self.top += dy
        self.bottom += dy
        return self
    @override
    def scale(
        self,
        new_weight: NumberLike,
        new_height: NumberLike
        ):
        center_x = self.left + self.get_width()/2.0
        center_y = self.bottom + self.get_height()/2.0
        self.left = center_x - new_weight / 2
        self.right = center_x + new_weight / 2
        self.top = center_y - new_height / 2
        self.bottom = center_y + new_height / 2
        return self

    @override
    def make_intersection(self, other:abs_box):
        return Box(
            max(self.left,      other.get_left_edge()),
            min(self.right,     other.get_right_edge()),
            max(self.top,       other.get_top_edge()),
            min(self.bottom,    other.get_bottom_edge())
        )
    @override
    def make_large_union(self, other:abs_box):
        return Box(
            min(self.left,      other.get_left_edge()),
            max(self.right,     other.get_right_edge()),
            min(self.top,       other.get_top_edge()),
            max(self.bottom,    other.get_botto_edge())
        )

BoxLike = Union[
    Box,
    BoxBasicLike
]

RectBasicLike = Union[
    Tuple[NumberLike, NumberLike, NumberLike, NumberLike],
    Sequence[NumberLike],
    str,#json {x:..., }
    Dict[str, NumberLike],#json {x..., },
    np.ndarray
]

class Rect:
    def __init__(self, x: float, y: float, w: float, h: float):
        self.__inject_array = np.array([x, y, w, h])

    @property
    def x(self):
        return self.__inject_array[0]
    @x.setter
    def x(self, value):
        self.__inject_array[0] = value
    @property
    def y(self):
        return self.__inject_array[1]
    @y.setter
    def y(self, value):
        self.__inject_array[1] = value
    @property
    def w(self):
        return self.__inject_array[2]
    @w.setter
    def w(self, value):
        self.__inject_array[2] = value
    @property
    def h(self):
        return self.__inject_array[3]
    @h.setter
    def h(self, value):
        self.__inject_array[3] = value
    @property
    def data_array(self):
        return self.__inject_array
    @data_array.setter
    def data_array(self, value):
        self.__inject_array = value

    @override
    def __str__(self):
        return f"{self.x}, {self.y}, {self.w}, {self.h}"
    @override
    def to_json(self):
        return {"x": self.x, "y": self.y, "w": self.w, "h": self.h}
    @override
    def to_tuple(self):
        return (self.x, self.y, self.w, self.h)
    @override
    def to_list(self):
        return [self.x, self.y, self.w, self.h]
    def to_box(self):
        return Box(self.x, self.x + self.w, self.y, self.y + self.h)
    def from_box(self, box:Box):
        self.x = box.left
        self.y = box.top
        self.w = box.get_width()
        self.h = box.get_height()
        return self
    @override
    def __repr__(self):
        return f"Rect({self.x}, {self.y}, {self.w}, {self.h})"

    def __getitem__(self, key):
        if key == 0 or key == "x":
            return self.x
        elif key == 1 or key == "y":
            return self.y
        elif key == 2 or key == "w" or key == "width":
            return self.w
        elif key == 3 or key == "h" or key == "height":
            return self.h
        else:
            raise IndexError("Index out of range")
    def __setitem__(self, key, value):
        if key == 0 or key == "x":
            self.x = value
        elif key == 1 or key == "y":
            self.y = value
        elif key == 2 or key == "w" or key == "width":
            self.w = value
        elif key == 3 or key == "h" or key == "height":
            self.h = value
        else:
            raise IndexError("Index out of range")

    @property
    def datasize(self):
        return (self.w, self.h)

    @override
    def get_left_edge(self):
        return self.x
    @override
    def get_right_edge(self):
        return self.x + self.w
    @override
    def get_top_edge(self):
        return self.h
    @override
    def get_bottom_edge(self):
        return self.y

    @override
    def make_intersection(self, other:Self) -> Self:
        return Rect(
            max(self.x, other.get_left_edge()),
            max(self.y, other.get_top_edge()),
            min(self.x + self.w, other.get_right_edge()) - max(self.x, other.get_left_edge()),
            min(self.y + self.h, other.get_bottom_edge()) - max(self.y, other.get_top_edge())
        )
    @override
    def make_large_union(self, other:Self) -> Self:
        return Rect(
            min(self.x, other.get_left_edge()),
            min(self.y, other.get_top_edge()),
            max(self.x + self.w, other.get_right_edge()) - min(self.x, other.get_left_edge()),
            max(self.y + self.h, other.get_bottom_edge()) - min(self.y, other.get_top_edge())
        )

RectLike = Union[Rect, RectBasicLike]

OriginBoxType = Union[RectLike, BoxLike]

def Wrapper2Box(
    datahead:   Union[
        NumberLike,
        BoxLike,
        RectLike
        ],
    right:  Optional[NumberLike] = None,
    top:    Optional[NumberLike] = None,
    bottom: Optional[NumberLike] = None
    ) -> Box:
    if right is not None:
        return Box(datahead, right, top, bottom)

    if isinstance(datahead, np.ndarray):
        return Box(datahead[0], datahead[1], datahead[2], datahead[3])
    if isinstance(datahead, BoxLike):
        if isinstance(datahead, Box):
            return datahead
        else:
            return Box(*datahead)
    if isinstance(datahead, RectLike):
        if isinstance(datahead, Rect):
            return datahead.to_box()
        else:
            return Rect(*datahead).to_box()
    return Box(datahead, right, top, bottom)

def Wrapper2Rect(
    datahead:  Union[
        NumberLike,
        BoxLike,
        RectLike
        ],
    y:  Optional[NumberLike] = None,
    w:  Optional[NumberLike] = None,
    h:  Optional[NumberLike] = None,
    /
    ) -> Rect:
    if y is not None:
        return Rect(datahead, y, w, h)

    if isinstance(datahead, np.ndarray):
        return Rect(datahead[0], datahead[1], datahead[2], datahead[3])
    if isinstance(datahead, RectLike):
        if isinstance(datahead, Rect):
            return datahead
        else:
            return Rect(*datahead)
    if isinstance(datahead, BoxLike):
        if isinstance(datahead, Box):
            return Rect(datahead.left, datahead.bottom, datahead.right-datahead.left, datahead.top-datahead.bottom)
        else:
            temp = Box(*datahead)
            return Rect(temp.left, temp.bottom, temp.right-temp.left, temp.top-temp.bottom)
    return Rect(datahead, y, w, h)

# region end

# region quaternion

class Quaternion:
    def __init__(self, r:NumberLike, i:NumberLike, j:NumberLike, k:NumberLike):
        self.__inject_array = np.array([r, i, j, k])

    @property
    def r(self):
        return self.__inject_array[0]
    @r.setter
    def r(self, value:NumberLike):
        self.__inject_array[0] = value
    @property
    def i(self):
        return self.__inject_array[1]
    @i.setter
    def i(self, value:NumberLike):
        self.__inject_array[1] = value
    @property
    def j(self):
        return self.__inject_array[2]
    @j.setter
    def j(self, value:NumberLike):
        self.__inject_array[2] = value
    @property
    def k(self):
        return self.__inject_array[3]
    @k.setter
    def k(self, value:NumberLike):
        self.__inject_array[3] = value
    @property
    def data_array(self):
        return self.__inject_array
    @data_array.setter
    def data_array(self, value:Sequence):
        self.__inject_array = np.array(value, dtype=float)

    def __repr__(self):
        return f"Quaternion({self.__inject_array[0]}, {self.__inject_array[1]}i, {self.__inject_array[2]}j, {self.__inject_array[3]}k)"

    def __add__(self, other):
        return Quaternion(*(self.__inject_array + other.data))

    def __sub__(self, other):
        return Quaternion(*(self.__inject_array - other.data))

    def __mul__(self, other):
        a1, b1, c1, d1 = self.__inject_array
        a2, b2, c2, d2 = other.data
        return Quaternion(
            a1*a2 - b1*b2 - c1*c2 - d1*d2,
            a1*b2 + b1*a2 + c1*d2 - d1*c2,
            a1*c2 - b1*d2 + c1*a2 + d1*b2,
            a1*d2 + b1*c2 - c1*b2 + d1*a2
        )

    def conjugate(self):
        return Quaternion(self.__inject_array[0], -self.__inject_array[1], -self.__inject_array[2], -self.__inject_array[3])

    def norm(self):
        return np.linalg.norm(self.__inject_array)

    def inverse(self):
        return self.conjugate() * (1.0 / self.norm()**2)

if __name__ == "__main__":
    # 示例使用
    q1 = Quaternion(1, 2, 3, 4)
    q2 = Quaternion(5, 6, 7, 8)

    print("q1:", q1)
    print("q2:", q2)

    print("q1 + q2:", q1 + q2)
    print("q1 - q2:", q1 - q2)
    print("q1 * q2:", q1 * q2)
    print("Conjugate of q1:", q1.conjugate())
    print("Norm of q1:", q1.norm())
    print("Inverse of q1:", q1.inverse())








