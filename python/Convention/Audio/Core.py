import sounddevice as base
import soundfile as sf
from ..File.Core import tool_file

def list_devices():
    """
    列出所有音频设备
    """
    return base.query_devices()

def save_audio_to_wav(audio_data, file_path, samplerate=44100):
    """
    将音频数据保存为 WAV 格式
    :param audio_data: 音频数据
    :param file_path: 文件路径
    """
    sf.write(file_path, audio_data, samplerate)

def convert_wav_to(wav_file_path:str, output_file_path:str)->None:
    """
    将 WAV 文件转换为 MP3 格式
    :param wav_file_path: WAV 文件路径
    :param output_file_path: 输出 文件路径
    """
    file = tool_file(wav_file_path)
    file.load_as_wav()
    file.save_as_audio(output_file_path)

def play_audio(file_path):
    """
    播放音频文件
    :param file_path: 音频文件路径
    """
    data, samplerate = sf.read(file_path)
    base.play(data, samplerate)

def record_audio(file_path, duration:int):
    """
    录制音频文件
    :param file_path: 音频文件路径
    :param duration: 录制时长（秒）
    """
    data = base.rec(duration=duration)
    save_audio_to_wav(data, file_path)

def stop_audio():
    """
    停止播放音频文件
    :param file_path: 音频文件路径
    """
    base.stop()

def get_audio_duration(file_path):
    """
    获取音频文件时长
    :param file_path: 音频文件路径
    """
    return sf.info(file_path).duration

def get_audio_samplerate(file_path):
    """
    获取音频文件采样率
    :param file_path: 音频文件路径
    """
    return sf.info(file_path).samplerate

def get_audio_channels(file_path):

    """
    获取音频文件声道数
    :param file_path: 音频文件路径
    """
    return sf.info(file_path).channels

def get_audio_format(file_path):
    """
    获取音频文件格式
    :param file_path: 音频文件路径
    """
    return sf.info(file_path).format

def convert_audio_format(input_data, output_type:str, tool_temp_path:str=".temp_convert_audio_path"):
    """
    转换音频文件格式
    :param input_data: 输入数据
    :param input_type: 输入格式
    :param output_type: 输出格式
    :param tool_temp_path: 临时文件路径
    """
    file = tool_file(tool_temp_path+("." if output_type[0] is not '.' else '')+output_type)
    file.data = input_data
    file.save_as_audio()
    result = file.refresh().data
    file.remove()
    return result

def convert_audio_format_with_file(input_file_path:str, output_file_path:str, output_type:str):
    """
    转换音频文件格式
    :param input_file_path: 输入文件路径
    :param output_file_path: 输出文件路径
    :param output_type: 输出格式
    """
    file = tool_file(input_file_path)
    file.load()
    file.save_as_audio(output_file_path, output_type)
    return file.refresh().data