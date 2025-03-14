from typing             import *
from ..Internal     import *

import cv2              as     base
import cv2.data         as     BaseData
from ..MathEx.Core  import *
from PIL                import ImageFile, Image

from ..Str.Core     import UnWrapper as Unwrapper2Str
from ..File.Core    import tool_file, Wrapper as Wrapper2File, tool_file_or_str, loss_file

# OpenCV Image format is BGR
# PIL Image format is RBG

VideoWriter = base.VideoWriter
def mp4_with_MPEG4_fourcc() -> int:
    return VideoWriter.fourcc(*"mp4v")
def avi_with_Xvid_fourcc() -> int:
    return VideoWriter.fourcc(*"XVID")
def avi_with_DivX_fourcc() -> int:
    return VideoWriter.fourcc(*"DIVX")
def avi_with_MJPG_fourcc() -> int:
    return VideoWriter.fourcc(*"MJPG")
def mp4_or_avi_with_H264_fourcc() -> int:
    return VideoWriter.fourcc(*"X264")
def avi_with_H265_fourcc() -> int:
    return VideoWriter.fourcc(*"H264")
def wmv_with_WMV1_fourcc() -> int:
    return VideoWriter.fourcc(*"WMV1")
def wmv_with_WMV2_fourcc() -> int:
    return VideoWriter.fourcc(*"WMV2")
def oggTheora_with_THEO_fourcc() -> int:
    return VideoWriter.fourcc(*"THEO")
def flv_with_FLV1_fourcc() -> int:
    return VideoWriter.fourcc(*"FLV1")
class VideoWriterInstance(VideoWriter, any_class):
    def __init__(
        self,
        file_name:  Union[tool_file, str],
        fourcc:     int,
        fps:        float,
        frame_size: tuple[int, int],
        is_color:   bool = True
        ):
        if isinstance(file_name, loss_file):
            raise ValueError(f"file_name<{file_name.SymbolName()}> is not a valid file")
        super().__init__(Unwrapper2Str(file_name), fourcc, fps, frame_size, is_color)
    def __del__(self):
        self.release()

AffineFeature_feature2D = base.AffineFeature.create
SIFT_Feature2D = base.SIFT.create
ORB_Feature2D = base.ORB.create
BRISK_Feature2D = base.BRISK.create
AKAZE_Feature2D = base.AKAZE.create
KAZE_Feature2D = base.KAZE.create
MSER_Feature2D = base.MSER.create
FastFeatureDetector_Feature2D = base.FastFeatureDetector.create
AgastFeatureDetector_Feature2D = base.AgastFeatureDetector.create
GFTTDetector_Feature2D = base.GFTTDetector.create
SimpleBlobDetector_Feature2D = base.SimpleBlobDetector.create
class Feature2DInstance[featrue:base.Feature2D](left_value_reference[featrue]):
    def __init__(
        self,
        feature2D:Union[featrue, ClosuresCallable[featrue]]
        ):
        if isinstance(feature2D, base.Feature2D):
            super().__init__(feature2D)
        else:
            super().__init__(feature2D())
    def detect[Mat_or_Mats:Union[
            MatLike,
            base.UMat,
            Sequence[MatLike],
            Sequence[base.UMat]
        ]](
        self,
        image:  Mat_or_Mats,
        mask:   Optional[Mat_or_Mats] = None
        ) -> Sequence[base.KeyPoint]:
        return self.ref_value.detect(image, mask)
    def compute[Mat_or_Mats:Union[
            MatLike,
            base.UMat,
            Sequence[MatLike],
            Sequence[base.UMat]
        ]](
        self,
        image:          Mat_or_Mats,
        keypoints:      Optional[Sequence[base.KeyPoint]] = None,
        descriptors:    Optional[Mat_or_Mats] = None
        ) -> Tuple[Sequence[base.KeyPoint], MatLike]:
        return self.ref_value.compute(image, keypoints, descriptors)
    def detectAndCompute[_Mat:Union[
            MatLike,
            base.UMat,
        ]](
        self,
        image:          _Mat,
        mask:           Optional[_Mat] = None,
        descriptors:    Optional[_Mat] = None,
        useProvidedKeypoints:bool = False
        ) -> Tuple[Sequence[base.KeyPoint], MatLike]:
        return self.ref_value.detectAndCompute(image, mask, descriptors, useProvidedKeypoints)

def wait_key(delay:int):
    return base.waitKey(delay)
def until_esc():
    return wait_key(0)

def is_current_key(key:str, *, wait_delay:int = 1):
    return wait_key(wait_delay) & 0xFF == ord(key[0])

class light_cv_view(any_class):
    def __init__(self, filename_or_index:Union[str, tool_file, int]):
        self.__capture: base.VideoCapture   = None
        self.stats:     bool                = True
        self.retarget(filename_or_index)
    def __del__(self):
        self.release()

    @override
    def ToString(self):
        return f"View<{self.width}x{self.height}>"

    def __bool__(self):
        return self.stats

    def is_open(self):
        return self.__capture.isOpened()

    def release(self):
        if self.__capture is not None:
            self.__capture.release()
    def retarget(self, filename_or_index:Union[str, tool_file, int]):
        self.release()
        if isinstance(filename_or_index, int):
            self.__capture = base.VideoCapture(filename_or_index)
        elif isinstance(filename_or_index, loss_file):
            raise ValueError(f"filename_or_index<{filename_or_index.SymbolName()}> is not a valid file")
        else:
            self.__capture = base.VideoCapture(Unwrapper2Str(filename_or_index))
        return self

    def next_frame(self) -> MatLike:
        self.stats, frame =self.__capture.read()
        if self.stats:
            return frame
        else:
            return None

    def get_captrue_info(self, id:int):
        return self.__capture.get(id)
    def get_prop_pos_msec(self):
        return self.get_captrue_info(0)
    def get_prop_pos_frames(self):
        return self.get_captrue_info(1)
    def get_prop_avi_ratio(self):
        return self.get_captrue_info(2)
    def get_prop_frame_width(self):
        return self.get_captrue_info(3)
    def get_prop_frame_height(self):
        return self.get_captrue_info(4)
    def get_prop_fps(self):
        return self.get_captrue_info(5)
    def get_prop_fourcc(self):
        return self.get_captrue_info(6)
    def get_prop_frame_count(self):
        return self.get_captrue_info(7)
    def get_prop_format(self):
        return self.get_captrue_info(8)
    def get_prop_mode(self):
        return self.get_captrue_info(9)
    def get_prop_brightness(self):
        return self.get_captrue_info(10)
    def get_prop_contrast(self):
        return self.get_captrue_info(11)
    def get_prop_saturation(self):
        return self.get_captrue_info(12)
    def get_prop_hue(self):
        return self.get_captrue_info(13)
    def get_prop_gain(self):
        return self.get_captrue_info(14)
    def get_prop_exposure(self):
        return self.get_captrue_info(15)
    def get_prop_convert_rgb(self):
        return self.get_captrue_info(16)

    def setup_capture(self, id:int, value):
        self.__capture.set(id, value)
        return self
    def set_prop_pos_msec(self, value:int):
        return self.setup_capture(0, value)
    def set_prop_pos_frames(self, value:int):
        return self.setup_capture(1, value)
    def set_prop_avi_ratio(self, value:float):
        return self.setup_capture(2, value)
    def set_prop_frame_width(self, value:int):
        return self.setup_capture(3, value)
    def set_prop_frame_height(self, value:int):
        return self.setup_capture(4, value)
    def set_prop_fps(self, value:int):
        return self.setup_capture(5, value)
    def set_prop_fourcc(self, value):
        return self.setup_capture(6, value)
    def set_prop_frame_count(self, value):
        return self.setup_capture(7, value)
    def set_prop_format(self, value):
        return self.setup_capture(8, value)
    def set_prop_mode(self, value):
        return self.setup_capture(9, value)
    def set_prop_brightness(self, value):
        return self.setup_capture(10, value)
    def set_prop_contrast(self, value):
        return self.setup_capture(11, value)
    def set_prop_saturation(self, value):
        return self.setup_capture(12, value)
    def set_prop_hue(self, value):
        return self.setup_capture(13, value)
    def set_prop_gain(self, value):
        return self.setup_capture(14, value)
    def set_prop_exposure(self, value):
        return self.setup_capture(15, value)
    def set_prop_convert_rgb(self, value:int):
        return self.setup_capture(16, value)
    def set_prop_rectification(self, value:int):
        return self.setup_capture(17, value)

    @property
    def width(self) -> float:
        return self.get_prop_frame_width()
    @width.setter
    def width(self, value:float) -> float:
        self.set_prop_frame_width(value)
        return value
    @property
    def height(self):
        return self.get_prop_frame_height()
    @height.setter
    def height(self, value:float) -> float:
        self.set_prop_frame_height(value)
        return value

    @property
    def frame_size(self) -> Tuple[float, float]:
        return self.get_prop_frame_width(), self.get_prop_frame_height()
    @property
    def shape(self):
        return self.frame_size
    @frame_size.setter
    def frame_size(self, value:Tuple[float, float]) -> Tuple[float, float]:
        self.set_prop_frame_width(value[0])
        self.set_prop_frame_height(value[1])
        return value

class light_cv_camera(light_cv_view, any_class):
    def __init__(self, index:int = 0):
        self.writer:    VideoWriter = None
        super().__init__(int(index))

    @override
    def release(self):
        super().release()
        if self.writer is not None:
            self.writer.release()

    def current_frame(self):
        return self.next_frame()

    def recording(
        self,
        stop_pr:    Callable[[], bool],
        writer:     Union[VideoWriter, Callable[[MatLike], Any]],
        ):
        writer_stats = False
        if isinstance(writer, VideoWriter):
            self.writer = writer
            writer_stats = True
        while self.is_open():
            if stop_pr():
                break
            frame = self.current_frame()
            base.imshow("__recording__", frame)
            if writer_stats:
                writer.write(frame)
            else:
                writer(frame)
        base.destroyWindow("__recording__")
        return self

    @override
    def ToString(self):
        return f"Camera<{self.width}x{self.height}>"

def get_zero_mask(shape, *args, **kwargs) -> MatLike:
    return np.zeros(shape, *args, **kwargs)
def get_one_mask(shape, value, *args, **kwargs) -> MatLike:
    return np.ones(shape, value, *args, **kwargs)

class ImageObject(left_np_ndarray_reference):
    @property
    def __image(self) -> MatLike:
        return self.ref_value
    @__image.setter
    def __image(self, value:MatLike) -> MatLike:
        self.ref_value = value
        return value

    def __init__(
        self,
        image:          Optional[Union[
            str,
            Self,
            light_cv_camera,
            tool_file,
            MatLike,
            np.ndarray,
            ImageFile.ImageFile,
            Image.Image
            ]],
        flags:          int             = -1):
        super().__init__()
        self.__camera:  light_cv_camera = None
        self.current:   MatLike         = None
        self.__gray:    MatLike         = None
        if isinstance(image, light_cv_camera):
            self.lock_from_camera(image)
        else:
            self.load_image(image, flags)

    def internal_check_when_image_is_none_throw_error(self):
        if self.image is None:
            raise ValueError("Image is None")
        return self

    @override
    def SymbolName(self):
        return "Image"
    @override
    def ToString(self):
        current = self.image
        if current is None:
            return "null"
        return f"Image<{current.shape[1]}x{current.shape[0]}:\n"+str(
            self.image)+"\n>"

    @property
    def camera(self) -> light_cv_camera:
        if self.__camera is None or self.__camera.is_open() is False:
            return None
        else:
            return self.__camera
    @property
    def image(self) -> MatLike:
        if self.current is not None:
            return self.current
        elif self.camera is None:
            return self.__image
        else:
            return self.__camera.current_frame()

    @image.setter
    def image(self, image:          Optional[Union[
            str,
            Self,
            tool_file,
            MatLike,
            np.ndarray,
            ImageFile.ImageFile,
            Image.Image
            ]]):
        self.load_image(image)

    def load_from_nparray(
        self,
        array_: np.ndarray,
        code:   int = base.COLOR_RGB2BGR,
        *args, **kwargs
        ):
        self.__gray = None
        self.__image = base.cvtColor(array_, code, *args, **kwargs).astype(np.uint8)
        return self
    def load_from_PIL_image(
        self,
        image:  Image.Image,
        code:   int = base.COLOR_RGB2BGR,
        *args, **kwargs
    ):
        self.load_from_nparray(np.array(image), code, *args, **kwargs)
        return self
    def load_from_PIL_ImageFile(
        self,
        image:  ImageFile.ImageFile,
        rect:   Optional[Tuple[float, float, float, float]] = None
    ):
        return self.load_from_PIL_image(image.crop(rect))
    def load_from_cv2_image(self, image:  MatLike):
        self.__gray = None
        self.__image = image
        return self
    def lock_from_camera(self, camera: light_cv_camera):
        self.__camera = camera
        return self

    @property
    def height(self) -> int:
        return self.shape[0]
    @property
    def width(self) -> int:
        return self.shape[1]
    @property
    def channel_depth(self) -> int:
        return self.shape[2]

    @property
    def pixel_count(self) -> int:
        return self.image.size
    @property
    def dtype(self):
        return self.image.dtype

    def is_enable(self):
        return self.image is not None
    def is_invalid(self):
        return self.is_enable() is False
    def __bool__(self):
        return self.is_enable()
    def __MatLike__(self):
        return self.image

    def load_image(
        self,
        image:          Optional[Union[
            str,
            tool_file,
            Self,
            MatLike,
            np.ndarray,
            ImageFile.ImageFile,
            Image.Image
            ]],
        flags:          int = -1
        ):
        """加载图片"""
        if image is None:
            self.__image = None
            return self
        elif isinstance(image, ImageObject):
            self.__image = image.image
        elif isinstance(image, MatLike):
            self.__image = image
        elif isinstance(image, np.ndarray):
            self.load_from_nparray(image, flags)
        elif isinstance(image, ImageFile.ImageFile):
            self.load_from_PIL_ImageFile(image, flags)
        elif isinstance(image, Image.Image):
            self.load_from_PIL_image(image, flags)
        elif isinstance(image, loss_file):
            self.__image = None
        else:
            self.__image = base.imread(Unwrapper2Str(image), flags)
        return self
    def save_image(self, save_path:Union[str, tool_file], is_path_must_exist = False):
        """保存图片"""
        if isinstance(save_path, loss_file):
            return self
        if is_path_must_exist:
            Wrapper2File(save_path).try_create_parent_path()
        if self.is_enable():
            base.imwrite(Unwrapper2Str(save_path), self.image)
        return self

    def show_image(
        self,
        window_name:        str                         = "Image",
        delay:              Union[int,str]              = 0,
        image_show_func:    Callable[[Self], None]      = None,
        *args, **kwargs
        ):
        """显示图片"""
        if self.is_invalid():
            return self
        if self.camera is not None:
            while (wait_key(1) & 0xFF != ord(str(delay)[0])) and self.camera is not None:
                # dont delete this line, self.image is camera flame now, see<self.current = None>
                self.current = self.image
                if image_show_func is not None:
                    image_show_func(self)
                if self.current is not None:
                    base.imshow(window_name, self.current)
                # dont delete this line, see property<image>
                self.current = None
        else:
            base.imshow(window_name, self.image)
            base.waitKey(delay = int(delay), *args, **kwargs)
        if base.getWindowProperty(window_name, base.WND_PROP_VISIBLE) > 0:
            base.destroyWindow(window_name)
        return self

    # 绝对值转换
    def convert_scale_abs(self):
        """绝对值转换"""
        return ImageObject(base.convertScaleAbs(self.image))

    # 图像边缘检测
    def edge_detect_with_sobel(
        self,
        *,
        ksize:      Optional[int]   = None,
        scale:      Optional[float] = None,
        delta:      Optional[float] = None,
        borderType: Optional[int]   = None,
        **kwargs
        ):
        if ksize is not None:
            kwargs["ksize"] = ksize
        if scale is not None:
            kwargs["scale"] = scale
        if delta is not None:
            kwargs["delta"] = delta
        if borderType is not None:
            kwargs["borderType"] = borderType
        gray = self.get_grayscale()
        dx = base.Sobel(gray, base.CV_16S, 1, 0, **kwargs)
        dy = base.Sobel(gray, base.CV_16S, 0, 1, **kwargs)
        return ImageObject(dx).convert_scale_abs().merge_with_blending(
                ImageObject(dy).convert_scale_abs(), (0.5, 0.5))
    def edge_detect_with_roberts(self):
        kernelx = np.array([[-1,0],[0,1]], dtype=int)
        kernely = np.array([[0,-1],[1,0]], dtype=int)
        gray = self.get_grayscale()
        dx = base.filter2D(gray, base.CV_16S, kernelx)
        dy = base.filter2D(gray, base.CV_16S, kernely)
        return ImageObject(dx).convert_scale_abs().merge_with_blending(
                ImageObject(dy).convert_scale_abs(), (0.5, 0.5))
    def edge_detect_with_laplacian(
        self,
        kernalSize: int = 3
        ):
        gray = self.get_grayscale()
        return ImageObject(base.convertScaleAbs(
            base.Laplacian(gray, base.CV_16S, ksize=kernalSize)
            ))
    def edge_detect_with_canny(
        self,
        threshold1: float,
        threshold2: float,
        **kwargs
        ):
        return ImageObject(base.Canny(
            self.get_grayscale(), threshold1, threshold2, **kwargs
            ))

    # 分离通道
    def split(self):
        """分离通道"""
        return base.split(self.image)
    def split_to_image_object(self):
        """分离通道"""
        return [ImageObject(channel) for channel in self.split()]
    @property
    def channels(self):
        return self.split()
    @property
    def blue_channel(self):
        return self.channels[0]
    @property
    def green_channel(self):
        return self.channels[1]
    @property
    def red_channel(self):
        return self.channels[2]
    @property
    def alpha_channel(self):
        return self.channels[3]
    def get_blue_image(self):
        return ImageObject(self.blue_channel)
    def get_green_image(self):
        return ImageObject(self.green_channel)
    def get_red_image(self):
        return ImageObject(self.red_channel)
    def get_alpha_image(self):
        return ImageObject(self.alpha_channel)

    # 混合通道
    def merge_channels_from_list(self, channels:List[MatLike]):
        """合并通道"""
        self.image = base.merge(channels)
        return self
    def merge_channels(self, blue:MatLike, green:MatLike, red:MatLike):
        """合并通道"""
        return self.merge_channels_from_list([blue, green, red])
    def merge_channel_list(self, bgr:List[MatLike]):
        """合并通道"""
        return self.merge_channels_from_list(bgr)

    # Transform
    def get_resize_image(self, width:int, height:int):
        if self.is_enable():
            return ImageObject(base.resize(self.image, (width, height)))
        return None
    def get_rotate_image(self, angle:float):
        if self.is_invalid():
            return None
        (h, w) = self.image.shape[:2]
        center = (w // 2, h // 2)
        M = base.getRotationMatrix2D(center, angle, 1.0)
        return ImageObject(base.warpAffine(self.image, M, (w, h)))
    def resize_image(self, width:int, height:int):
        """调整图片大小"""
        new_image = self.get_resize_image(width, height)
        if new_image is not None:
            self.image = new_image.image
        return self
    def rotate_image(self, angle:float):
        """旋转图片"""
        new_image = self.get_rotate_image(angle)
        if new_image is not None:
            self.image = new_image.image
        return self

    # 图片翻折
    def flip(self, flip_code:int):
        """翻转图片"""
        if self.is_enable():
            self.image = base.flip(self.image, flip_code)
        return self
    def horizon_flip(self):
        """水平翻转图片"""
        return self.flip(1)
    def vertical_flip(self):
        """垂直翻转图片"""
        return self.flip(0)
    def both_flip(self):
        """双向翻转图片"""
        return self.flip(-1)

    # 色彩空间猜测
    def guess_color_space(self) -> str:
        """猜测色彩空间"""
        if self.is_invalid():
            return None
        image = self.image
        # 计算每个通道的像素值分布
        hist_b = base.calcHist([image], [0], None, [256], [0, 256])
        hist_g = base.calcHist([image], [1], None, [256], [0, 256])
        hist_r = base.calcHist([image], [2], None, [256], [0, 256])

        # 计算每个通道的像素值总和
        sum_b = np.sum(hist_b)
        sum_g = np.sum(hist_g)
        sum_r = np.sum(hist_r)

        # 根据像素值总和判断色彩空间
        if sum_b > sum_g and sum_b > sum_r:
            #print("The image might be in BGR color space.")
            return "BGR"
        elif sum_g > sum_b and sum_g > sum_r:
            #print("The image might be in GRAY color space.")
            return "GRAY"
        else:
            #print("The image might be in RGB color space.")
            return "RGB"

    # 颜色转化
    def get_convert(self, color_convert:int):
        """颜色转化"""
        if self.is_invalid():
            return None
        return ImageObject(base.cvtColor(self.image, color_convert))
    def convert_to(self, color_convert:int):
        """颜色转化"""
        if self.is_invalid():
            return None
        self.image = self.get_convert(color_convert)
        return self

    def is_grayscale(self):
        return self.dimension == 2
    def get_grayscale(self, curColor=base.COLOR_BGR2GRAY) -> MatLike:
        if self.is_invalid():
            return None
        if self.__gray is None and self.camera is None:
            self.__gray = base.cvtColor(self.image, curColor)
        return self.__gray
    def convert_to_grayscale(self):
        """将图片转换为灰度图"""
        self.__image = self.get_grayscale()
        return self

    def get_convert_flag(
        self,
        targetColorTypeName:Literal[
            "BGR", "RGB", "GRAY", "YCrCb"
            ]
        ) -> Optional[int]:
        """获取颜色转化标志"""
        flag = self.guess_color_space()
        if flag is None:
            return None

        if targetColorTypeName == "BGR":
            if flag == "RGB":
                return base.COLOR_RGB2BGR
            elif flag == "GRAY":
                return base.COLOR_GRAY2BGR
            elif flag == "YCrCb":
                return base.COLOR_YCrCb2BGR
        elif targetColorTypeName == "RGB":
            if flag == "BGR":
                return base.COLOR_BGR2RGB
            elif flag == "GRAY":
                return base.COLOR_GRAY2RGB
            elif flag == "YCrCb":
                return base.COLOR_YCrCb2RGB
        elif targetColorTypeName == "GRAY":
            if flag == "RGB":
                return base.COLOR_RGB2GRAY
            elif flag == "RGB":
                return base.COLOR_BGR2GRAY
        return None

    # 原址裁切
    def sub_image(self, x:int, y:int ,width:int ,height:int):
        """裁剪图片"""
        if self.is_invalid():
            return self
        self.image = self.image[y:y+height, x:x+width]
        return self

    # 直方图
    def equalizeHist(self, is_cover = False) -> MatLike:
        """直方图均衡化"""
        if self.is_invalid():
            return self
        result:MatLike = base.equalizeHist(self.image)
        if is_cover:
            self.image = result
        return result
    def calcHist(
        self,
        channel:    Union[List[int], int],
        mask:       Optional[MatLike]       = None,
        hist_size:  Sequence[int]           = [256],
        ranges:     Sequence[float]         = [0, 256]
        ) -> MatLike:
        """计算直方图"""
        if self.is_invalid():
            return None
        return base.calcHist(
            [self.image],
            channel if isinstance(channel, list) else [channel],
            mask,
            hist_size,
            ranges)

    # 子集操作
    def sub_image_with_rect(self, rect:Tuple[float, float, float, float]):
        """裁剪图片"""
        if self.is_invalid():
            return self
        self.image = self.image[rect[1]:rect[1]+rect[3], rect[0]:rect[0]+rect[2]]
        return self
    def sub_image_with_box(self, box:Tuple[float, float, float, float]):
        """裁剪图片"""
        if self.is_invalid():
            return self
        self.image = self.image[box[1]:box[3], box[0]:box[2]]
        return self
    def sub_cover_with_rect(self, image:Union[Self, MatLike], rect:Tuple[float, float, float, float]):
        """覆盖图片"""
        if self.is_invalid():
            raise ValueError("Real Image is none")
        if isinstance(image, MatLike):
            image = ImageObject(image)
        self.image[rect[1]:rect[1]+rect[3], rect[0]:rect[0]+rect[2]] = image.image
        return self
    def sub_cover_with_box(self, image:Union[Self, MatLike], box:Tuple[float, float, float, float]):
        """覆盖图片"""
        if self.is_invalid():
            raise ValueError("Real Image is none")
        if isinstance(image, MatLike):
            image = ImageObject(image)
        self.image[box[1]:box[3], box[0]:box[2]] = image.image
        return self

    def operator_cv(self, func:Callable[[MatLike], Any], *args, **kwargs):
        func(self.image, *args, **kwargs)
        return self

    # 序列合并
    @override
    def _inject_stack_uniform_item(self):
        return np.uint8(self.image)

    # 从另一图像合并
    def merge_with_blending(self, other:Self, weights:Tuple[float, float]):
        return ImageObject(base.addWeighted(self.image, weights[0], other.image, weights[1], 0))
    # 从另一图像合并(遮罩)
    def merge_with_mask(self, other:Self, mask:Self):
        return ImageObject(base.bitwise_and(self.image, other.image, mask.image))

    # 滤波
    def filter(self, ddepth:int, kernel:MatLike, *args, **kwargs):
        return base.filter2D(self.image, ddepth, kernel, *args, **kwargs)
    def filter_blur(self, kernalSize:Tuple[float, float]):
        return base.blur(self.image, kernalSize)
    def filter_gaussian(self, kernalSize:Tuple[float, float], sigmaX:float, sigmaY:float):
        return base.GaussianBlur(self.image, kernalSize, sigmaX, sigmaY)
    def filter_median(self, kernalSize:int):
        return base.medianBlur(self.image, kernalSize)
    def filter_bilateral(self, d:float, sigmaColor:float, sigmaSpace:float):
        return base.bilateralFilter(self.image, d, sigmaColor, sigmaSpace)
    def filter_sobel(self, dx:int, dy:int, kernalSize:int):
        return base.Sobel(self.image, -1, dx, dy, ksize=kernalSize)
    def filter_canny(self, threshold1:float, threshold2:float):
        return base.Canny(self.image, threshold1, threshold2)
    def filter_laplacian(self, kernalSize:int):
        return base.Laplacian(self.image, -1, ksize=kernalSize)
    def filter_scharr(self, dx:int, dy:int):
        return base.Scharr(self.image, -1, dx, dy)
    def filter_box_blur(self, kernalSize:Tuple[float, float]):
        return base.boxFilter(self.image, -1, ksize=kernalSize, normalize=0)

    # 阈值
    def threshold(
        self,
        threshold:float,
        type:int
        ):
        return base.threshold(self.image, threshold, 255, type)
    def adaptiveThreshold(
        self,
        adaptiveMethod: int = base.ADAPTIVE_THRESH_MEAN_C,
        thresholdType:  int = base.THRESH_BINARY,
        blockSize:      int = 11,
        C:            float = 2,
        ):
        return base.adaptiveThreshold(self.image, 255, adaptiveMethod, thresholdType, blockSize, C)
    # 获取二值化
    def Separate2EnableScene(self,*, is_front=True, is_back=False):
        '''
        return mask -> front, back
        '''
        if is_back == is_front:
            is_back = not is_front
        gray = self.get_grayscale()
        if is_front:
            return base.threshold(gray, 255.0/2.0, 255, base.THRESH_BINARY)
        else:
            return base.threshold(gray, 255.0/2.0, 255, base.THRESH_BINARY_INV)
    def Separate2EnableScenes_with_Otsu(self,*, is_front=True, is_back=False):
        '''
        return mask -> front, back
        '''
        if is_back == is_front:
            is_back = not is_front
        gray = self.get_grayscale()
        if is_front:
            return base.threshold(gray, 0, 255, base.THRESH_BINARY | base.THRESH_OTSU)
        else:
            return base.threshold(gray, 0, 255, base.THRESH_BINARY_INV | base.THRESH_OTSU)
    # 获取二值化遮罩
    def SeparateFrontBackScenes(self):
        '''
        return mask -> front, back
        '''
        gray = self.get_grayscale()
        _, front = base.threshold(gray, 255.0/2.0, 255, base.THRESH_BINARY)
        _, back = base.threshold(gray, 255.0/2.0, 255, base.THRESH_BINARY_INV)
        return np.where(gray>=front), np.where(gray>=back)
    def SeparateFrontBackScenes_with_Otsu(self):
        '''
        return mask -> front, back
        '''
        gray = self.get_grayscale()
        _, front = base.threshold(gray, 0, 255, base.THRESH_BINARY | base.THRESH_OTSU)
        _, back = base.threshold(gray, 0, 255, base.THRESH_BINARY_INV | base.THRESH_OTSU)
        return np.where(gray>=front), np.where(gray>=back)
    # 获取核
    def get_kernel(self, shape:int, kernalSize:Tuple[float, float]):
        return base.getStructuringElement(shape, kernalSize)
    def get_rect_kernal(self, kernalSize:Tuple[float, float]):
        return self.get_kernel(base.MORPH_RECT, kernalSize)
    def get_cross_kernal(self, kernalSize:Tuple[float, float]):
        return self.get_kernel(base.MORPH_CROSS, kernalSize)
    def get_ellipse_kernal(self, kernalSize:Tuple[float, float]):
        return self.get_kernel(base.MORPH_ELLIPSE, kernalSize)
    # 膨胀
    def dilate(self, kernel:Optional[MatLike]=None, *args, **kwargs):
        if kernel is None:
            kernel = self.get_rect_kernal((3, 3))
        return base.dilate(self.image, kernel, *args, **kwargs)
    # 腐蚀
    def erode(self, kernel:Optional[MatLike]=None, *args, **kwargs):
        if kernel is None:
            kernel = self.get_rect_kernal((3, 3))
        return base.erode(self.image, kernel, *args, **kwargs)
    # 开运算
    def open_operator(self, kernel:Optional[MatLike]=None, *args, **kwargs):
        if kernel is None:
            kernel = self.get_rect_kernal((3, 3))
        return base.morphologyEx(self.image, base.MORPH_OPEN, kernel, *args, **kwargs)
    # 闭运算
    def close_operator(self, kernel:Optional[MatLike]=None, *args, **kwargs):
        if kernel is None:
            kernel = self.get_rect_kernal((3, 3))
        return base.morphologyEx(self.image, base.MORPH_CLOSE, kernel, *args, **kwargs)
    # 梯度运算
    def gradient_operator(self, kernel:Optional[MatLike]=None, *args, **kwargs):
        if kernel is None:
            kernel = self.get_rect_kernal((3, 3))
        return base.morphologyEx(self.image, base.MORPH_GRADIENT, kernel, *args, **kwargs)
    # 顶帽运算
    def tophat_operator(self, kernel:Optional[MatLike]=None, *args, **kwargs):
        if kernel is None:
            kernel = self.get_rect_kernal((3, 3))
        return base.morphologyEx(self.image, base.MORPH_TOPHAT, kernel, *args, **kwargs)
    # 黑帽运算
    def blackhat_operator(self, kernel:Optional[MatLike]=None, *args, **kwargs):
        if kernel is None:
            kernel = self.get_rect_kernal((3, 3))
        return base.morphologyEx(self.image, base.MORPH_BLACKHAT, kernel, *args, **kwargs)

    # 绘制轮廓
    def drawContours(
        self,
        contours:   Sequence[MatLike],
        contourIdx: int                         = -1,
        color:      Union[MatLike, Tuple[int]]  = (0, 0, 0),
        thickness:  int                         = 1,
        lineType:   int                         = base.LINE_8,
        hierarchy:  Optional[MatLike]           = None,
        maxLevel:   int                         = base.FILLED,
        offset:     Optional[Point]             = None,
        is_draw_on_self:bool                    = False
        ) -> MatLike:
        image = self.image if is_draw_on_self else self.image.copy()
        return base.drawContours(image, contours, contourIdx, color, thickness, lineType, hierarchy, maxLevel, offset)
    # 修改自身的绘制
    def draw_rect(
        self,
        rect:       Rect,
        color:      Union[MatLike, Tuple[int]]  = (0, 0, 0),
        thickness:  int                         = 1,
        lineType:   int                         = base.LINE_8,
        ) -> MatLike:
        base.rectangle(self.image, rect, color, thickness, lineType)
        return self
    # 获取轮廓
    def get_contours(
        self,
        *,
        mode:       int                         = base.RETR_LIST,
        method:     int                         = base.CHAIN_APPROX_SIMPLE,
        is_front:   bool                        = True,
        contours:   Optional[Sequence[MatLike]] = None,
        hierarchy:  Optional[MatLike]           = None,
        offset:     Optional[Point]             = None
        ) -> Tuple[Sequence[MatLike], MatLike]:
        _, bin = self.Separate2EnableScene(is_front=is_front)
        if offset is not None:
            return base.findContours(bin, mode, method, contours, hierarchy, offset)
        else:
            return base.findContours(bin, mode, method, contours, hierarchy)
    def get_contours_mask(
        self,
        width:      int,
        *,
        mode:       int                         = base.RETR_LIST,
        method:     int                         = base.CHAIN_APPROX_SIMPLE,
        is_front:   bool                        = True,
        contours:   Optional[Sequence[MatLike]] = None,
        hierarchy:  Optional[MatLike]           = None,
        offset:     Optional[Point]             = None,
        ) -> Tuple[Sequence[MatLike], MatLike]:
        find_contours, _ = self.get_contours(
            mode=mode,
            method=method,
            is_front=is_front,
            contours=contours,
            hierarchy=hierarchy,
            offset=offset
            )
        return base.drawContours(get_zero_mask(self.shape, dtype=np.uint8), find_contours, -1, (255, 255, 255), width)
    def get_contours_fill_inside_mask(
        self,
        *,
        mode:       int                         = base.RETR_LIST,
        method:     int                         = base.CHAIN_APPROX_SIMPLE,
        is_front:   bool                        = True,
        contours:   Optional[Sequence[MatLike]] = None,
        hierarchy:  Optional[MatLike]           = None,
        offset:     Optional[Point]             = None
        ) -> Tuple[Sequence[MatLike], MatLike]:
        return self.get_contours_mask(
            mode=mode,
            method=method,
            is_front=is_front,
            contours=contours,
            hierarchy=hierarchy,
            offset=offset,
            width=-1
            )
    # 获取轮廓方框
    def get_xy_rect_from_contours(
        self,
        *,
        mode:       int                         = base.RETR_LIST,
        method:     int                         = base.CHAIN_APPROX_SIMPLE,
        is_front:   bool                        = True,
        contours:   Optional[Sequence[MatLike]] = None,
        hierarchy:  Optional[MatLike]           = None,
        offset:     Optional[Point]             = None
        ) -> Sequence[Rect]:
        return [base.boundingRect(contour) for contour in self.get_contours(
            mode=mode,
            method=method,
            is_front=is_front,
            contours=contours,
            hierarchy=hierarchy,
            offset=offset
            )]
    def get_minarea_rect_from_contours(
        self,
        *,
        mode:       int                         = base.RETR_LIST,
        method:     int                         = base.CHAIN_APPROX_SIMPLE,
        is_front:   bool                        = True,
        contours:   Optional[Sequence[MatLike]] = None,
        hierarchy:  Optional[MatLike]           = None,
        offset:     Optional[Point]             = None
        ) -> Sequence[RotatedRect]:
        return [base.minAreaRect(contour) for contour in self.get_contours(
            mode=mode,
            method=method,
            is_front=is_front,
            contours=contours,
            hierarchy=hierarchy,
            offset=offset)]

    # 图像匹配
    def match_on_scene(
        self,
        scene_image:        Self,
        # Feature2D config
        featrue_type:       Optional[Union[
            base.Feature2D,
            ClosuresCallable[base.Feature2D],
            Feature2DInstance
        ]]                                              = SIFT_Feature2D,
        optout_feature_kp_and_des_ref:
                            Optional[left_value_reference[
            Tuple[Sequence[base.KeyPoint], MatLike]
        ]]                                              = None,
        # Match Config
        match_min_points:   int                         = 4,
        # Draw rect Config
        rect_color:         Tuple[int, int, int]        = (0, 255, 0),
        rect_thickness:     int                         = 2,
        # Draw match Config
        out_drawMatches_ref:Optional[left_value_reference[MatLike]
        ]                                               = None,
        drawMatches_range:  Optional[Tuple[int, int]]   = None
        ) -> MatLike:
        '''
        本图像作为目标特征

        Args
        ---
        Target Image
            scene_image:
                识别的场景, 此图像将作为目标匹配的场景

        Feature2D Config
            type:
                特征检测器类型/生成器/实例, 默认为SIFT
            ref:
                左值引用容器, ref_value为空时将用于存储特征点与描述符,
                不为空则提取ref_value作为本次的特征点与描述符

        Match Config
            min_points:
                匹配的最小点数, 小于此值则无法找到目标物体

        Draw rect Config
            color:
                矩形框颜色, 默认为绿色
            thickness:
                矩形框厚度, 默认为2

            Draw match Config
                ref:
                    左值引用容器, 将用于存储合成的对比图像
                range:
                    匹配点的绘制范围(靠前的匹配度高), 默认为全部绘制

        Return
        ---
        MatLike: 绘制了方框的矩形
        '''
        # 读取目标图和场景图
        target_img = self.get_grayscale()
        scene_img = scene_image.get_grayscale()

        # 初始化SIFT检测器
        feature2D:Feature2DInstance = featrue_type if (
            isinstance(featrue_type, Feature2DInstance)
        ) else Feature2DInstance(featrue_type)

        # 检测关键点和描述符
        kp1:Sequence[base.KeyPoint] = None
        des1:MatLike = None
        if optout_feature_kp_and_des_ref is None:
            kp1, des1 = feature2D.detectAndCompute(target_img, None)
        else:
            if optout_feature_kp_and_des_ref.ref_value is None:
                kp1, des1 = feature2D.detectAndCompute(target_img, None)
                optout_feature_kp_and_des_ref.ref_value = (kp1, des1)
            else:
                kp1, des1 = optout_feature_kp_and_des_ref.ref_value
        kp2, des2 = feature2D.detectAndCompute(scene_img, None)

        # 初始化BFMatcher
        bf = base.BFMatcher(base.NORM_L2, crossCheck=True)

        # 匹配描述符
        matches = bf.match(des1, des2)
        matches = sorted(matches, key=lambda x: x.distance)

        # 如果匹配点数少于min_match_points个，无法找到目标物体
        if len(matches) < match_min_points:
            return None

        # 提取匹配点的位置
        src_pts = np.float32([kp1[m.queryIdx].pt for m in matches]).reshape(-1, 1, 2)
        dst_pts = np.float32([kp2[m.trainIdx].pt for m in matches]).reshape(-1, 1, 2)

        # 使用RANSAC算法找到单应性矩阵
        M, _ = base.findHomography(src_pts, dst_pts, base.RANSAC, 5.0)

        # 获取目标物体的边界框
        h, w = target_img.shape
        pts = np.float32([[0, 0], [0, h - 1], [w - 1, h - 1], [w - 1, 0]]).reshape(-1, 1, 2)
        dst = base.perspectiveTransform(pts, M)

        # 在scene中绘制边界框
        result = scene_image.image.copy()
        base.polylines(
            result,
            [np.int32(dst)],
            True,
            rect_color,
            rect_thickness,
            base.LINE_AA
            )

        # 合成的绘制结果
        if out_drawMatches_ref is not None:
            if drawMatches_range is None:
                out_drawMatches_ref.ref_value = base.drawMatches(
                self.image,
                kp1,
                scene_image.image,
                kp2,
                matches,
                None, flags=base.DrawMatchesFlags_NOT_DRAW_SINGLE_POINTS
            )
            else:
                out_drawMatches_ref.ref_value = base.drawMatches(
                self.image,
                kp1,
                scene_image.image,
                kp2,
                matches[drawMatches_range[0]:drawMatches_range[1]],
                None, flags=base.DrawMatchesFlags_NOT_DRAW_SINGLE_POINTS
            )

        # 返回绘制了方框的结果
        return result

    # np加速

def get_new_noise(
    raw_image:  Optional[MatLike],
    height:     int,
    weight:     int,
    *,
    mean:       float   = 0,
    sigma:      float   = 25,
    dtype               = np.uint8
    ) -> MatLike:
    noise = raw_image
    if noise is None:
        noise = np.zeros((height, weight), dtype=dtype)
    base.randn(noise, mean, sigma)
    return base.cvtColor(noise, base.COLOR_GRAY2BGR)
class NoiseImageObject(ImageObject):
    def __init__(
        self,
        height:     int,
        weight:     int,
        *,
        mean:       float   = 0,
        sigma:      float   = 25,
        dtype               = np.uint8
        ):
        super().__init__(get_new_noise(
            None, height, weight, mean=mean, sigma=sigma, dtype=dtype
            ))

    @override
    def SymbolName(self):
        return "Noise"

def Unwrapper(image:Optional[Union[
            str,
            ImageObject,
            tool_file,
            MatLike,
            np.ndarray,
            ImageFile.ImageFile,
            Image.Image
            ]]) -> MatLike:
    return image.image if isinstance(image, ImageObject) else ImageObject(image).image

def Wrapper(image:Optional[Union[
            str,
            ImageObject,
            tool_file,
            MatLike,
            np.ndarray,
            ImageFile.ImageFile,
            Image.Image
            ]]) -> ImageObject:
    return ImageObject(image)

class light_cv_window:
    def __init__(self, name:str):
        self.__my_window_name = name
        base.namedWindow(self.__my_window_name)
    def __del__(self):
        self.destroy()

    def show_image(self, image:Union[ImageObject, MatLike]):
        if self.__my_window_name is None:
            self.__my_window_name = "window"
        if isinstance(image, ImageObject):
            image = image.image
        base.imshow(self.__my_window_name, image)
        return self
    def destroy(self):
        if self.__my_window_name is not None and base.getWindowProperty(self.__my_window_name, base.WND_PROP_VISIBLE) > 0:
            base.destroyWindow(self.__my_window_name)
        return self

    @property
    def window_rect(self):
        return base.getWindowImageRect(self.__my_window_name)
    @window_rect.setter
    def window_rect(self, rect:Tuple[float, float, float, float]):
        self.set_window_rect(rect[0], rect[1], rect[2], rect[3])

    def set_window_size(self, weight:int, height:int):
        base.resizeWindow(self.__my_window_name, weight, height)
        return self
    def get_window_size(self) -> Tuple[float, float]:
        rect = self.window_rect
        return rect[2], rect[3]

    def get_window_property(self, prop_id:int):
        return base.getWindowProperty(self.__my_window_name, prop_id)
    def set_window_property(self, prop_id:int, prop_value:int):
        base.setWindowProperty(self.__my_window_name, prop_id, prop_value)
        return self
    def get_prop_frame_width(self):
        return self.window_rect[2]
    def get_prop_frame_height(self):
        return self.window_rect[3]
    def is_full_window(self):
        return base.getWindowProperty(self.__my_window_name, base.WINDOW_FULLSCREEN) > 0
    def set_full_window(self):
        base.setWindowProperty(self.__my_window_name, base.WINDOW_FULLSCREEN, 1)
        return self
    def set_normal_window(self):
        base.setWindowProperty(self.__my_window_name, base.WINDOW_FULLSCREEN, 0)
        return self
    def is_using_openGL(self):
        return base.getWindowProperty(self.__my_window_name, base.WINDOW_OPENGL) > 0
    def set_using_openGL(self):
        base.setWindowProperty(self.__my_window_name, base.WINDOW_OPENGL, 1)
        return self
    def set_not_using_openGL(self):
        base.setWindowProperty(self.__my_window_name, base.WINDOW_OPENGL, 0)
        return self
    def is_autosize(self):
        return base.getWindowProperty(self.__my_window_name, base.WINDOW_AUTOSIZE) > 0
    def set_autosize(self):
        base.setWindowProperty(self.__my_window_name, base.WINDOW_AUTOSIZE, 1)
        return self
    def set_not_autosize(self):
        base.setWindowProperty(self.__my_window_name, base.WINDOW_AUTOSIZE, 0)
        return self

    def set_window_rect(self, x:int, y:int, weight:int, height:int):
        base.moveWindow(self.__my_window_name, x, y)
        return self.set_window_size(weight, height)

    def set_window_pos(self, x:int, y:int):
        base.moveWindow(self.__my_window_name, x, y)
        return self

    def wait_key(self, wait_time:int=0):
        return base.waitKey(wait_time)

def get_haarcascade_frontalface(name_or_default:Optional[str]=None):
    if name_or_default is None:
        name_or_default = "haarcascade_frontalface_default"
    return base.CascadeClassifier(BaseData.haarcascades+'haarcascade_frontalface_default.xml')

def detect_human_face(
    image:          ImageObject,
    detecter:       base.CascadeClassifier,
    scaleFactor:    float                   = 1.1,
    minNeighbors:   int                     = 4,
    *args, **kwargs):
    '''return is Rect[]'''
    return detecter.detectMultiScale(image.image, scaleFactor, minNeighbors, *args, **kwargs)

class internal_detect_faces_oop(Callable[[ImageObject], None]):
    def __init__(self):
        self.face_cascade = get_haarcascade_frontalface()
    def __call__(self, image:ImageObject):
        gray = image.convert_to_grayscale()
        faces = self.face_cascade.detectMultiScale(gray, 1.3, 5)
        for (x,y,w,h) in faces:
            image.operator_cv(base.rectangle,(x,y),(x+w,y+h),(255,0,0),2)

def easy_detect_faces(camera:light_cv_camera):
    ImageObject(camera).show_image("window", 'q', internal_detect_faces_oop())

# 示例使用
if __name__ == "__main__":
    img_obj = ImageObject("path/to/your/image.jpg")
    img_obj.show_image()
    img_obj.resize_image(800, 600)
    img_obj.rotate_image(45)
    img_obj.convert_to_grayscale()
    img_obj.save_image("path/to/save/image.jpg")

# Override tool_file to tool_file_ex

class tool_file_cvex(tool_file):
    def __init__(self,  file_path:str, *args, **kwargs):
        super().__init__(file_path, *args, **kwargs)

    @override
    def load_as_image(self) -> ImageObject:
        self.data = ImageObject(self)
        return self.data
    @override
    def save_as_image(self, path = None):
        image:ImageObject   = self.data
        image.save_image(path if path is not None else self.get_path())
        return self

def WrapperFile2CVEX(file:Union[tool_file_or_str, tool_file_cvex]):
    if isinstance(file, tool_file_cvex):
        return file
    elif isinstance(file, str):
        return tool_file_cvex(file)
    elif isinstance(file, tool_file):
        result = tool_file_cvex(Unwrapper2Str(file))
        result.data = file.data
        return result
    else:
        raise TypeError("file must be tool_file or str")