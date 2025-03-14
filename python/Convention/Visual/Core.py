from typing                 import *
from pydantic               import BaseModel
from abc                    import *
import                             random
import numpy                as     np
import matplotlib.pyplot    as     plt
import seaborn              as     sns
from ..Internal         import *
from ..MathEx.Core      import *
from ..Str.Core         import UnWrapper as Unwrapper2Str
from ..File.Core        import tool_file, Wrapper as Wrapper2File, tool_file_or_str, is_image_file, loss_file, static_loss_file_dir
from ..Visual.OpenCV    import ImageObject, tool_file_cvex, WrapperFile2CVEX, Wrapper as Wrapper2Image, get_new_noise
from PIL.Image              import Image as PILImage
from PIL.ImageFile          import ImageFile as PILImageFile
import cv2                  as     cv2

class data_visual_generator:
    def __init__(self, file:tool_file_or_str):
        self._file:tool_file = Wrapper2File(file)
        self._file.load()

    def open(self, mode='r', is_refresh=False, encoding:str='utf-8', *args, **kwargs):
        self._file.open(mode, is_refresh, encoding, *args, **kwargs)

    def reload(self, file:Optional[tool_file_or_str]):
        if file is not None:
            self._file = Wrapper2File(file)
        self._file.load()


    def plot_line(self, x, y, df=None, title="折线图", x_label=None, y_label=None):
        plt.figure(figsize=(10, 6))
        sns.lineplot(data=df if df is not None else self._file.data, x=x, y=y)
        plt.title(title)
        plt.xlabel(x_label if x_label is not None else str(x))
        plt.ylabel(y_label if y_label is not None else str(y))
        plt.grid(True)
        plt.show()

    def plot_bar(self, x, y, df=None, figsize=(10,6), title="柱状图", x_label=None, y_label=None):
        plt.figure(figsize=figsize)
        sns.barplot(data=df if df is not None else self._file.data, x=x, y=y)
        plt.title(title)
        plt.xlabel(x_label if x_label is not None else str(x))
        plt.ylabel(y_label if y_label is not None else str(y))
        plt.grid(True)
        plt.show()

    def plot_scatter(self, x, y, df=None, title="散点图", x_label=None, y_label=None):
        plt.figure(figsize=(10, 6))
        sns.scatterplot(data=df if df is not None else self._file.data, x=x, y=y)
        plt.title(title)
        plt.xlabel(x_label if x_label is not None else str(x))
        plt.ylabel(y_label if y_label is not None else str(y))
        plt.grid(True)
        plt.show()

    def plot_histogram(self, column, df=None, title="直方图", x_label=None, y_label=None):
        plt.figure(figsize=(10, 6))
        sns.histplot(data=df if df is not None else self._file.data, x=column)
        plt.title(title)
        plt.xlabel(x_label if x_label is not None else str(column))
        plt.ylabel(y_label if y_label is not None else "value")
        plt.grid(True)
        plt.show()

    def plot_pairplot(self, df=None, title="成对关系图"):
        sns.pairplot(df if df is not None else self._file.data)
        plt.suptitle(title, y=1.02)
        plt.show()

    def plot_pie(self, column, figsize=(10,6), df=None, title="饼图"):
        plt.figure(figsize=figsize)
        if df is not None:
            df[column].value_counts().plot.pie(autopct='%1.1f%%')
        else:
            self._file.data[column].value_counts().plot.pie(autopct='%1.1f%%')
        plt.title(title)
        plt.ylabel('')  # 移除y轴标签
        plt.show()

    def plot_box(self, x, y, df=None, figsize=(10,6), title="箱线图", x_label=None, y_label=None):
        plt.figure(figsize=figsize)
        sns.boxplot(data=df if df is not None else self._file.data, x=x, y=y)
        plt.title(title)
        plt.xlabel(x_label if x_label is not None else str(x))
        plt.ylabel(y_label if y_label is not None else str(y))
        plt.grid(True)
        plt.show()

    def plot_heatmap(self, df=None, figsize=(10,6), title="热力图", cmap='coolwarm'):
        plt.figure(figsize=figsize)
        sns.heatmap(df.corr() if df is not None else self._file.data.corr(), annot=True, cmap=cmap)
        plt.title(title)
        plt.show()

    def plot_catplot(self, x, y, hue=None, df=None, kind='bar', figsize=(10,6), title="分类数据图", x_label=None, y_label=None):
        plt.figure(figsize=figsize)
        sns.catplot(data=df if df is not None else self._file.data, x=x, y=y, hue=hue, kind=kind)
        plt.title(title)
        plt.xlabel(x_label if x_label is not None else str(x))
        plt.ylabel(y_label if y_label is not None else str(y))
        plt.grid(True)
        plt.show()
    def plot_catplot_strip(self, x, y, hue=None, df=None, figsize=(10,6), title="分类数据图", x_label=None, y_label=None):
        self.plot_catplot(x, y, hue=hue, df=df, kind='strip', figsize=figsize, title=title, x_label=x_label, y_label=y_label)
    def plot_catplot_swarm(self, x, y, hue=None, df=None, figsize=(10,6), title="分类数据图", x_label=None, y_label=None):
        self.plot_catplot(x, y, hue=hue, df=df, kind='swarm', figsize=figsize, title=title, x_label=x_label, y_label=y_label)
    def plot_catplot_box(self, x, y, hue=None, df=None, figsize=(10,6), title="分类数据图", x_label=None, y_label=None):
        self.plot_catplot(x, y, hue=hue, df=df, kind='box', figsize=figsize, title=title, x_label=x_label, y_label=y_label)
    def plot_catplot_violin(self, x, y, hue=None, df=None, figsize=(10,6), title="分类数据图", x_label=None, y_label=None):
        self.plot_catplot(x, y, hue=hue, df=df, kind='violin', figsize=figsize, title=title, x_label=x_label, y_label=y_label)

    def plot_jointplot(self, x, y, kind="scatter", df=None, title="联合图", x_label=None, y_label=None):
        sns.jointplot(data=df if df is not None else self._file.data, x=x, y=y, kind=kind)
        plt.suptitle(title, y=1.02)
        plt.xlabel(x_label if x_label is not None else str(x))
        plt.ylabel(y_label if y_label is not None else str(y))
        plt.show()
    def plot_jointplot_scatter(self, x, y, df=None, title="联合图", x_label=None, y_label=None):
        self.plot_jointplot(x, y, kind="scatter", df=df, title=title, x_label=x_label, y_label=y_label)
    def plot_jointplot_kde(self, x, y, df=None, title="联合图", x_label=None, y_label=None):
        self.plot_jointplot(x, y, kind="kde", df=df, title=title, x_label=x_label, y_label=y_label)
    def plot_jointplot_hex(self, x, y, df=None, title="联合图", x_label=None, y_label=None):
        self.plot_jointplot(x, y, kind="hex", df=df, title=title, x_label=x_label, y_label=y_label)

class data_math_virsual_generator(data_visual_generator):
    def drop_missing_values(self, axis):
        """删除缺失值"""
        self._file.data = self._file.data.dropna(axis=axis)

    def fill_missing_values(self, value):
        """填充缺失值"""
        self._file.data = self._file.data.fillna(value)

    def remove_duplicates(self):
        """删除重复值"""
        self._file.data = self._file.data.drop_duplicates()

    def standardize_data(self):
        """数据标准化"""
        self._file.data = (self._file.data - self._file.data.mean()) / self._file.data.std()

    def normalize_data(self):
        """数据归一化"""
        self._file.data = (self._file.data - self._file.data.min()) / (self._file.data.max() - self._file.data.min())

# region image augmentation

NDARRAY_ANY = TypeVar("numpy.ndarray")
class BasicAugmentConfig(BaseModel, ABC):
    name:   str = "unknown"
    @abstractmethod
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        '''
        result:
            (change config, image)
        '''
        raise NotImplementedError()
class ResizeAugmentConfig(BasicAugmentConfig):
    width:      Optional[int]   = None
    height:     Optional[int]   = None
    name:       str             = "resize"
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        width = self.width
        height = self.height
        if width is None and height is None:
            rangewidth = origin.width
            rangeheight = origin.height
            width = rangewidth + random.randint(
                (-rangewidth*(random.random()%1)).__floor__(),
                (rangewidth*(random.random()%1)).__floor__()
                )
            height = rangeheight + random.randint(
                (-rangeheight*(random.random()%1)).__floor__(),
                (rangeheight*(random.random()%1)).__floor__()
                )
        elif width is None:
            width = origin.width
        elif height is None:
            height = origin.height
        change_config = {
            "width":width,
            "height":height
        }
        return (change_config, ImageObject(origin.get_resize_image(abs(width), abs(height))))
class ClipAugmentConfig(BasicAugmentConfig):
    mini:       Union[float, NDARRAY_ANY]   = 0
    maxi:       Union[float, NDARRAY_ANY]   = 255
    name:       str                         = "clip"
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        mini = self.mini
        maxi = self.maxi
        if isinstance(mini, ImageObject):
            mini = mini.get_array()
        if isinstance(maxi, ImageObject):
            maxi = maxi.get_array()
        change_config = {
            "mini":mini,
            "maxi":maxi
        }
        return (change_config, ImageObject(origin.clip(mini, maxi)))
class NormalizeAugmentConfig(BasicAugmentConfig):
    name:       str                                  = "normalize"
    mini:       Optional[Union[NDARRAY_ANY, float]]  = 0
    maxi:       Optional[Union[NDARRAY_ANY, float]]  = 255
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        change_config = {
            "mini":self.mini,
            "maxi":self.maxi
        }
        return (change_config, ImageObject(origin.normalize(self.mini, self.maxi)))
class StandardizeAugmentConfig(BasicAugmentConfig):
    name:       str                                 = "standardize"
    mean:       Optional[Union[NDARRAY_ANY, float]] = 0
    std:        Optional[Union[NDARRAY_ANY, float]] = 1
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        change_config = {
            "mean":origin.get_array().mean(),
            "std":origin.get_array().std()
        }
        return (change_config, ImageObject(origin.standardize(self.mean, self.std)))
class FlipAugmentConfig(BasicAugmentConfig):
    name:       str                 = "flip"
    axis:       Literal[-1, 1, 0]   = 1
    '''
    1:
        vertical
    0:
        horizontal
    -1:
        both
    '''
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        change_config = {
            "axis":self.axis
        }
        return (change_config, ImageObject(origin.flip(self.axis)))
class CropAugmentConfig(BasicAugmentConfig):
    name:       str     = "crop"
    lbx:        Optional[int]     = None
    lby:        Optional[int]     = None
    width:      Optional[int]     = None
    height:     Optional[int]     = None
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        lbx = self.lbx if self.lbx is not None else random.randint(0, origin.width)
        lby = self.lby if self.lby is not None else random.randint(0, origin.height)
        width = self.width if self.width is not None else random.randint(1, origin.width - lbx)
        height = self.height if self.height is not None else random.randint(0, origin.height - lby)
        change_config = {
            "lbx":lbx,
            "lby":lby,
            "width":width,
            "height":height
        }
        return (change_config, ImageObject(origin.sub_image_with_rect((lbx, lby, width, height))))
class FilterAugmentConfig(BasicAugmentConfig):
    name:       str             = "filter"
    ddepth:     int             = -1
    kernal:     NDARRAY_ANY     = cv2.getGaussianKernel(3, 1)
    def get_gaussian_kernal(self, kernal_size: int, sigma: float):
        return cv2.getGaussianKernel(kernal_size, sigma)
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        change_config = {
            "filter":self.ddepth,
            "kernal":self.kernal
        }
        return (change_config, ImageObject(origin.filter(self.ddepth, self.kernal)))
class ColorSpaceAugmentConfig(BasicAugmentConfig):
    name:       str     = "color_space"
    space:      int     = cv2.COLOR_BGR2GRAY
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        change_config = {
            "color_space":self.space
        }
        return (change_config, ImageObject(origin.convert_to(self.space)))
class LightingAugmentConfig(BasicAugmentConfig):
    name:       str             = "lighting"
    lighting:   Optional[int]   = None
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        lighting = self.lighting if self.lighting is not None else random.randint(0, 50)
        change_config = {
            "lighting":lighting
        }
        return (change_config, ImageObject(cv2.add(origin.image, lighting)))
class DarkingAugmentConfig(BasicAugmentConfig):
    name:       str                         = "darking"
    darking:    Optional[FloatBetween01]    = None
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        darking = self.darking if self.darking is not None else (random.random()%0.9+0.1)
        change_config = {
            "darking":darking
        }
        return (change_config, ImageObject(origin*darking))
class ContrastAugmentConfig(BasicAugmentConfig):
    name:       str                         = "contrast"
    contrast:   Optional[FloatBetween01]    = None
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        change_config = {
            "contrast":self.contrast
        }
        contrast = self.contrast if self.contrast is not None else (random.random()%0.9+0.1)
        contrast = int(contrast*255)
        result = origin.image*(contrast / 127 + 1) - contrast
        return (change_config, ImageObject(result))
class SeparateSceneAugmentConfig(BasicAugmentConfig):
    scene:      str     = "separate_scene"
    is_front:   bool    = True
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        change_config = {
            "is_front":self.is_front
        }
        front, back = origin.SeparateFrontBackScenes()
        target_0 = back if self.is_front else front
        image = origin.image.copy()
        image[target_0] = 0
        return (change_config, ImageObject(image))
class NoiseAugmentConfig(BasicAugmentConfig):
    name:       str     = "noise"
    mean:       float   = 0
    sigma:      float   = 25
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        change_config = {
            "mean":self.mean,
            "sigma":self.sigma
        }
        return (change_config, ImageObject(
            origin + get_new_noise(
                None,
                origin.height,
                origin.width,
                mean=self.mean,
                sigma=self.sigma
                )
            ))
class VignettingAugmentConfig(BasicAugmentConfig):
    name:           str     = "vignetting"
    ratio_min_dist: float   = 0.2
    range_vignette: Tuple[float, float] = (0.2, 0.8)
    random_sign:    bool    = False
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        change_config = {
            "ratio_min_dist":self.ratio_min_dist,
            "range_vignette":self.range_vignette,
            "random_sign":self.random_sign
        }
        h, w = origin.shape[:2]
        min_dist = np.array([h, w]) / 2 * np.random.random() * self.ratio_min_dist

        # 创建距离中心点在两个轴上的距离矩阵
        x, y = np.meshgrid(np.linspace(-w/2, w/2, w), np.linspace(-h/2, h/2, h))
        x, y = np.abs(x), np.abs(y)

        # 在两个轴上创建晕影遮罩
        x = (x - min_dist[0]) / (np.max(x) - min_dist[0])
        x = np.clip(x, 0, 1)
        y = (y - min_dist[1]) / (np.max(y) - min_dist[1])
        y = np.clip(y, 0, 1)

        # 获取随机晕影强度
        vignette = (x + y) / 2 * np.random.uniform(self.range_vignette[0], self.range_vignette[1])
        vignette = np.tile(vignette[..., None], [1, 1, 3])

        sign = 2 * (np.random.random() < 0.5) * (self.random_sign) - 1
        return (change_config, ImageObject(origin * (1 + sign * vignette)))
class LensDistortionAugmentConfig(BasicAugmentConfig):
    name:       str     = "lens_distortion"
    d_coef:     Tuple[
        float, float, float, float, float
        ] = (0.15, 0.15, 0.1, 0.1, 0.05)
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        change_config = {
            "d_coef":self.d_coef
        }
        # 获取图像的高度和宽度
        h, w = origin.shape[:2]

        # 计算对角线长度
        f = (h ** 2 + w ** 2) ** 0.5

        # 设置图像投影到笛卡尔坐标系的维度
        K = np.array([[f, 0, w / 2],
                      [0, f, h / 2],
                      [0, 0,     1]])

        d_coef = self.d_coef * np.random.random(5) # 值
        d_coef = d_coef * (2 * (np.random.random(5) < 0.5) - 1) # 符号
        # 从参数生成新的相机矩阵
        M, _ = cv2.getOptimalNewCameraMatrix(K, d_coef, (w, h), 0)

        # 生成用于重映射相机图像的查找表
        remap = cv2.initUndistortRectifyMap(K, d_coef, None, M, (w, h), 5)

        # 将原始图像重映射到新图像
        return (change_config, ImageObject(cv2.remap(origin.image, *remap, cv2.INTER_LINEAR)))
class RotationAugmentConfig(BasicAugmentConfig):
    name:       str     = "rotation"
    angle:      Optional[float]   = None
    scale:      float   = 1.0
    center:     Optional[Tuple[int, int]] = None
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        angle = self.angle if self.angle is not None else random.uniform(-30, 30)
        center = self.center if self.center is not None else (origin.width // 2, origin.height // 2)
        change_config = {
            "angle": angle,
            "scale": self.scale,
            "center": center
        }
        # 获取旋转矩阵
        rotation_matrix = cv2.getRotationMatrix2D(center, angle, self.scale)
        # 应用旋转变换
        rotated_image = cv2.warpAffine(origin.image, rotation_matrix, (origin.width, origin.height))
        return (change_config, ImageObject(rotated_image))
class BlurAugmentConfig(BasicAugmentConfig):
    name:       str     = "blur"
    kernel_size: Tuple[int, int] = (5, 5)
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        change_config = {
            "kernel_size": self.kernel_size
        }
        # 应用均值模糊
        blurred_image = cv2.blur(origin.image, self.kernel_size)
        return (change_config, ImageObject(blurred_image))
class MedianBlurAugmentConfig(BasicAugmentConfig):
    name:       str     = "median_blur"
    ksize:      int     = 5
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        change_config = {
            "ksize": self.ksize
        }
        # 应用中值模糊
        blurred_image = cv2.medianBlur(origin.image, self.ksize)
        return (change_config, ImageObject(blurred_image))
class SaturationAugmentConfig(BasicAugmentConfig):
    name:       str     = "saturation"
    factor:     Optional[float]   = None
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        factor = self.factor if self.factor is not None else random.uniform(0.5, 1.5)
        change_config = {
            "factor": factor
        }
        # 转换为HSV颜色空间
        hsv = cv2.cvtColor(origin.image, cv2.COLOR_BGR2HSV).astype(np.float32)
        # 调整饱和度
        hsv[:, :, 1] = hsv[:, :, 1] * factor
        hsv[:, :, 1] = np.clip(hsv[:, :, 1], 0, 255)
        # 转换回BGR颜色空间
        result = cv2.cvtColor(hsv.astype(np.uint8), cv2.COLOR_HSV2BGR)
        return (change_config, ImageObject(result))
class HueAugmentConfig(BasicAugmentConfig):
    name:       str     = "hue"
    shift:      Optional[int]   = None
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        shift = self.shift if self.shift is not None else random.randint(-20, 20)
        change_config = {
            "shift": shift
        }
        # 转换为HSV颜色空间
        hsv = cv2.cvtColor(origin.image, cv2.COLOR_BGR2HSV).astype(np.float32)
        # 调整色调
        hsv[:, :, 0] = (hsv[:, :, 0] + shift) % 180
        # 转换回BGR颜色空间
        result = cv2.cvtColor(hsv.astype(np.uint8), cv2.COLOR_HSV2BGR)
        return (change_config, ImageObject(result))
class GammaAugmentConfig(BasicAugmentConfig):
    name:       str     = "gamma"
    gamma:      Optional[float]   = None
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        gamma = self.gamma if self.gamma is not None else random.uniform(0.5, 2.0)
        change_config = {
            "gamma": gamma
        }
        # 应用伽马校正
        inv_gamma = 1.0 / gamma
        table = np.array([((i / 255.0) ** inv_gamma) * 255 for i in range(256)]).astype(np.uint8)
        result = cv2.LUT(origin.image, table)
        return (change_config, ImageObject(result))
class PerspectiveTransformAugmentConfig(BasicAugmentConfig):
    name:       str     = "perspective"
    intensity:  Optional[float]   = None
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        intensity = self.intensity if self.intensity is not None else random.uniform(0.05, 0.1)
        change_config = {
            "intensity": intensity
        }
        h, w = origin.shape[:2]

        # 定义源点和目标点
        src_points = np.float32([[0, 0], [w, 0], [0, h], [w, h]])

        # 随机扰动目标点
        dst_points = np.float32([
            [0 + random.uniform(-intensity * w, intensity * w), 0 + random.uniform(-intensity * h, intensity * h)],
            [w + random.uniform(-intensity * w, intensity * w), 0 + random.uniform(-intensity * h, intensity * h)],
            [0 + random.uniform(-intensity * w, intensity * w), h + random.uniform(-intensity * h, intensity * h)],
            [w + random.uniform(-intensity * w, intensity * w), h + random.uniform(-intensity * h, intensity * h)]
        ])

        # 计算透视变换矩阵
        M = cv2.getPerspectiveTransform(src_points, dst_points)

        # 应用透视变换
        result = cv2.warpPerspective(origin.image, M, (w, h))
        return (change_config, ImageObject(result))
class ElasticTransformAugmentConfig(BasicAugmentConfig):
    name:       str     = "elastic"
    alpha:      float   = 50
    sigma:      float   = 5
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        change_config = {
            "alpha": self.alpha,
            "sigma": self.sigma
        }
        h, w = origin.shape[:2]

        # 创建随机位移场
        dx = np.random.rand(h, w) * 2 - 1
        dy = np.random.rand(h, w) * 2 - 1

        # 高斯模糊位移场
        dx = cv2.GaussianBlur(dx, (0, 0), self.sigma)
        dy = cv2.GaussianBlur(dy, (0, 0), self.sigma)

        # 归一化并缩放
        dx = dx * self.alpha
        dy = dy * self.alpha

        # 创建网格
        x, y = np.meshgrid(np.arange(w), np.arange(h))

        # 应用位移
        indices_x = np.clip(x + dx, 0, w - 1).astype(np.float32)
        indices_y = np.clip(y + dy, 0, h - 1).astype(np.float32)

        # 重映射
        result = cv2.remap(origin.image, indices_x, indices_y, interpolation=cv2.INTER_LINEAR)
        return (change_config, ImageObject(result))
class ChannelShuffleAugmentConfig(BasicAugmentConfig):
    name:       str     = "channel_shuffle"
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        # 获取随机通道顺序
        channels = list(range(origin.image.shape[2]))
        random.shuffle(channels)
        change_config = {
            "channels": channels
        }
        # 重新排列通道
        result = origin.image[:, :, channels]
        return (change_config, ImageObject(result))
class MotionBlurAugmentConfig(BasicAugmentConfig):
    name:       str     = "motion_blur"
    kernel_size: int    = 15
    angle:      Optional[float]   = None
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        angle = self.angle if self.angle is not None else random.uniform(0, 360)
        change_config = {
            "kernel_size": self.kernel_size,
            "angle": angle
        }
        # 创建运动模糊核
        kernel = np.zeros((self.kernel_size, self.kernel_size))
        center = self.kernel_size // 2

        # 计算角度的弧度值
        rad = np.deg2rad(angle)

        # 在核上绘制一条线
        x = np.cos(rad) * center
        y = np.sin(rad) * center

        # 使用Bresenham算法绘制线
        cv2.line(kernel,
                (center - int(np.round(x)), center - int(np.round(y))),
                (center + int(np.round(x)), center + int(np.round(y))),
                1, thickness=1)

        # 归一化核
        kernel = kernel / np.sum(kernel)

        # 应用卷积
        result = cv2.filter2D(origin.image, -1, kernel)
        return (change_config, ImageObject(result))
class SolarizeAugmentConfig(BasicAugmentConfig):
    name:       str     = "solarize"
    threshold:  Optional[int]   = None
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        threshold = self.threshold if self.threshold is not None else random.randint(100, 200)
        change_config = {
            "threshold": threshold
        }
        # 应用曝光效果
        result = origin.image.copy()
        mask = origin.image > threshold
        result[mask] = 255 - result[mask]
        return (change_config, ImageObject(result))
class PosterizeAugmentConfig(BasicAugmentConfig):
    name:       str     = "posterize"
    bits:       Optional[int]   = None
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        bits = self.bits if self.bits is not None else random.randint(3, 7)
        change_config = {
            "bits": bits
        }
        # 应用海报效果（减少颜色位数）
        mask = 255 - (1 << (8 - bits))
        result = origin.image & mask
        return (change_config, ImageObject(result))
class InvertAugmentConfig(BasicAugmentConfig):
    name:       str     = "invert"
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        change_config = {}
        # 反转图像颜色
        result = 255 - origin.image
        return (change_config, ImageObject(result))
class EqualizationAugmentConfig(BasicAugmentConfig):
    name:       str     = "equalize"
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        change_config = {}
        # 对每个通道进行直方图均衡化
        result = origin.image.copy()
        if len(origin.shape) > 2 and origin.shape[2] > 1:
            for i in range(origin.shape[2]):
                result[:, :, i] = cv2.equalizeHist(origin.image[:, :, i])
        else:
            result = cv2.equalizeHist(origin.image)
        return (change_config, ImageObject(result))
class CutoutAugmentConfig(BasicAugmentConfig):
    name:       str     = "cutout"
    n_holes:    int     = 1
    length:     Optional[int]   = None
    @override
    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[Dict[str, Any], ImageObject]:
        length = self.length if self.length is not None else min(origin.width, origin.height) // 4
        change_config = {
            "n_holes": self.n_holes,
            "length": length
        }

        result = origin.image.copy()
        h, w = origin.shape[:2]

        for _ in range(self.n_holes):
            # 随机选择矩形的中心点
            y = np.random.randint(h)
            x = np.random.randint(w)

            # 计算矩形的边界
            y1 = np.clip(y - length // 2, 0, h)
            y2 = np.clip(y + length // 2, 0, h)
            x1 = np.clip(x - length // 2, 0, w)
            x2 = np.clip(x + length // 2, 0, w)

            # 将矩形区域填充为黑色
            result[y1:y2, x1:x2] = 0

        return (change_config, ImageObject(result))
# Config.name -> (field, value)
type ChangeConfig = Dict[str, Dict[str, Any]]
# (field, value)
type ResultImageObjects = Dict[str, ImageObject]
class ImageAugmentConfig(BaseModel):
    resize:     Optional[ResizeAugmentConfig]               = None
    clip:       Optional[ClipAugmentConfig]                 = None
    normalize:  Optional[NormalizeAugmentConfig]            = None
    standardize:Optional[StandardizeAugmentConfig]          = None
    flip:       Optional[FlipAugmentConfig]                 = None
    crop:       Optional[CropAugmentConfig]                 = None
    filters:    Sequence[FilterAugmentConfig]               = []
    colorspace: Optional[ColorSpaceAugmentConfig]           = None
    lighting:   Optional[LightingAugmentConfig]             = None
    darking:    Optional[DarkingAugmentConfig]              = None
    contrast:   Optional[ContrastAugmentConfig]             = None
    separate_scene: Literal[0, 1, 2, 3]                     = 0
    noise:      Optional[NoiseAugmentConfig]                = None
    vignette:   Optional[VignettingAugmentConfig]           = None
    lens_distortion: Optional[LensDistortionAugmentConfig]  = None
    rotation:   Optional[RotationAugmentConfig]             = None
    blur:       Optional[BlurAugmentConfig]                 = None
    median_blur:Optional[MedianBlurAugmentConfig]           = None
    saturation: Optional[SaturationAugmentConfig]           = None
    hue:        Optional[HueAugmentConfig]                  = None
    gamma:      Optional[GammaAugmentConfig]                = None
    perspective:Optional[PerspectiveTransformAugmentConfig] = None
    elastic:    Optional[ElasticTransformAugmentConfig]     = None
    channel_shuffle: Optional[ChannelShuffleAugmentConfig]  = None
    motion_blur:Optional[MotionBlurAugmentConfig]           = None
    solarize:   Optional[SolarizeAugmentConfig]             = None
    posterize:  Optional[PosterizeAugmentConfig]            = None
    invert:     Optional[InvertAugmentConfig]               = None
    '''
    None:
        0
    front:
        1
    back:
        2
    both:
        3
    '''
    log_call:   Optional[Callable[[Union[str, Dict[str, Any]]], None]] = None

    def get_all_configs(self) -> List[BasicAugmentConfig]:
        result = [
            self.resize,
            self.clip,
            self.normalize,
            self.standardize,
            self.flip,
            self.crop,
            self.colorspace,
            self.lighting,
            self.darking,
            self.contrast,
            self.noise,
            self.vignette,
            self.lens_distortion
            ]
        result.extend(self.filters)
        if self.separate_scene&1:
            result.append(SeparateSceneAugmentConfig(is_front=True, name="front_scene"))
        if self.separate_scene&2:
            result.append(SeparateSceneAugmentConfig(is_front=False, name="back_scene"))
        return result

    def _inject_log(self, *args, **kwargs):
        if self.log_call is not None:
            self.log_call(*args, **kwargs)

    def augment(
        self,
        origin:     ImageObject
        ) -> Tuple[ChangeConfig, ResultImageObjects]:
        result:                 Dict[str, ImageObject]      = {}
        result_change_config:   Dict[str, Dict[str, Any]]   = {}
        augment_configs:        List[BasicAugmentConfig]    = self.get_all_configs()
        for item in augment_configs:
            if item is not None:
                result_change_config[item.name], result[item.name] = item.augment(ImageObject(origin.image))
                self._inject_log(f"augmentation<{item.name}> change config: {result_change_config[item.name]}")
        return (result_change_config, result)
    def augment_to(
            self,
            input:      Union[tool_file, str, ImageObject, np.ndarray, PILImage, PILImageFile],
            output_dir: tool_file_or_str,
            *,
            # 如果输出目录不存在,将调用must_exist
            # 如果输出目录存在但不是目录,将返回父目录
            must_output_dir_exist:  bool                                = False,
            output_file_name:       str                                 = "output.png",
            callback:               Optional[Action[ChangeConfig]]      = None,
        ) -> ResultImageObjects:
        # 初始化环境和变量
        origin_image:   ImageObject = self.__init_origin_image(input)
        result_dir:     tool_file   = self.__init_result_dir(output_dir, must_output_dir_exist)
        # 增强
        self._inject_log(f"输出<{output_file_name}>开始增强")
        change_config, result = self._inject_augment(
            origin_image=origin_image,
            result_dir=result_dir,
            output_file_name=output_file_name,
        )
        # 结果
        if callback is not None:
            callback(change_config)
        return result
    def augment_from_dir_to(
            self,
            input_dir:  Union[tool_file, str],
            output_dir: tool_file_or_str,
            *,
            # 如果输出目录不存在,将调用must_exist
            # 如果输出目录存在但不是目录,将返回父目录
            must_output_dir_exist:  bool                                        = False,
            callback:               Optional[Action2[tool_file, ChangeConfig]]  = None,
        ) -> Dict[str, List[ImageObject]]:
        # 初始化环境和变量
        origin_images:  tool_file   = Wrapper2File(input_dir)
        result_dir:     tool_file   = self.__init_result_dir(output_dir, must_output_dir_exist)
        if origin_images.exists() is False or origin_images.is_dir() is False:
            raise FileExistsError(f"input_dir<{origin_images}> is not exist or not dir")
        # augment
        result: Dict[str, List[ImageObject]] = {}
        for image_file in origin_images.dir_tool_file_iter():
            if is_image_file(Unwrapper2Str(image_file)) is False:
                continue
            change_config, curResult = self._inject_augment(
                origin_image=WrapperFile2CVEX(image_file).load(),
                result_dir=result_dir,
                output_file_name=image_file.get_filename(),
            )
            # 添加单个结果
            for key in curResult:
                if key in result:
                    result[key].append(curResult[key])
                else:
                    result[key] = [curResult[key]]
            # 调用回调
            if callback is not None:
                callback(image_file, change_config)
        # 结果
        return result
    def augment_from_images_to(
            self,
            inputs:     Sequence[ImageObject],
            output_dir: tool_file_or_str,
            *,
            # 如果输出目录不存在,将调用must_exist
            # 如果输出目录存在但不是目录,将返回父目录
            must_output_dir_exist:  bool                                            = False,
            callback:               Optional[Action2[ImageObject, ChangeConfig]]    = None,
            fileformat:             str                                             = "{}.jpg",
            indexbuilder:           type                                            = int
        ) -> Dict[str, List[ImageObject]]:
        # Init env and vars
        result_dir:     tool_file   = self.__init_result_dir(output_dir, must_output_dir_exist)
        index:          Any         = indexbuilder()
        # augment
        result: Dict[str, List[ImageObject]] = {}
        for image in inputs:
            current_output_name = fileformat.format(index)
            change_config, curResult = self._inject_augment(
                origin_image=image,
                result_dir=result_dir,
                output_file_name=current_output_name,
            )
            # append single result
            for key in curResult:
                if key in result:
                    result[key].append(curResult[key])
                else:
                    result[key] = [curResult[key]]
            index += 1
            # call feedback
            if callback is not None:
                callback(image, change_config)
        # result
        return result
    def __init_origin_image(self, input:Union[tool_file, str, ImageObject, np.ndarray, PILImage, PILImageFile]) -> ImageObject:
        origin_image:   ImageObject = None
        # check
        if isinstance(input, (tool_file, str)):
            inputfile = WrapperFile2CVEX(input)
            if inputfile.data is not None:
                origin_image = inputfile.data
            else:
                origin_image = inputfile.load()
        elif isinstance(input, (ImageObject, np.ndarray, PILImage, PILImageFile)):
            origin_image = Wrapper2Image(input)
        else:
            raise TypeError(f"input<{input}> is not support type")
        return origin_image
    def __init_result_dir(self, output_dir:tool_file_or_str, must_output_dir_exist:bool) -> tool_file:
        if output_dir is None or isinstance(output_dir, loss_file):
            return static_loss_file_dir
        result_dir:     tool_file   = Wrapper2File(output_dir)
        # check exist
        stats:          bool        = True
        if result_dir.exists() is False:
            if must_output_dir_exist:
                result_dir.must_exists_path()
            else:
                stats = False
        if stats is False:
            raise FileExistsError(f"output_dir<{result_dir}> is not exist")
        # check dir stats
        if result_dir.is_dir() is False:
            if must_output_dir_exist:
                result_dir.back_to_parent_dir()
            else:
                raise FileExistsError(f"output_dir<{result_dir}> is not a dir")
        # result
        return result_dir

    def _inject_augment(
        self,
        origin_image:               ImageObject,
        result_dir:                 tool_file,
        output_file_name:           str
        ) -> Tuple[ChangeConfig, ResultImageObjects]:
        self._inject_log(f"output<{output_file_name}> is start augment")
        result_dict, result_images = self.augment(origin_image)
        if not (result_dir is None or isinstance(result_dir, loss_file)):
            for key, value in result_images.items():
                current_dir = result_dir|key
                current_result_file = current_dir|output_file_name
                value.save_image(current_result_file, True)
        return result_dict, result_images

def image_augent(
    config:ImageAugmentConfig,
    source,*args, **kwargs
    ):
    if isinstance(source, ImageObject):
        return config.augment(source, *args, **kwargs)

def get_config_of_gaussian_blur(
    intensity:  float,
    blur_level: int = 3
    ) -> FilterAugmentConfig:
    return FilterAugmentConfig(
        name="gaussian_blur",
        kernal=cv2.getGaussianKernel(blur_level, intensity)
    )
def get_config_of_smooth_blur(blur_level:int = 3):
    result = get_config_of_gaussian_blur(0, blur_level)
    result.name = "smooth_blur"
    return result
def get_config_of_sharpen(
    intensity:      float,
    sharpen_level:  float = 1
    ) -> FilterAugmentConfig:
    return FilterAugmentConfig(
        name="sharpen",
        kernal=np.array([
            [0, -sharpen_level, 0],
            [-sharpen_level, intensity-sharpen_level, -sharpen_level],
            [0, -sharpen_level, 0]
        ])
    )
def get_config_of_edge_enhance(
    intensity:      float,
    sharpen_level:  float = 8
    ) -> FilterAugmentConfig:
    return FilterAugmentConfig(
        name="edge_enhance",
        kernal=np.array([
            [-intensity, -intensity, -intensity],
            [-intensity, sharpen_level+intensity, -intensity],
            [-intensity, -intensity, -intensity]
        ])
    )
def get_config_of_convert_to_gray() -> ColorSpaceAugmentConfig:
    return ColorSpaceAugmentConfig(
        name="convert_to_gray",
        color_space=cv2.COLOR_BGR2GRAY
    )

# region end


