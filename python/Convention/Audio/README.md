# Audio 模块

Audio模块提供了音频处理、录制和语音识别等功能。

## 目录结构

- `Core.py`: 核心音频处理功能
- `Microphone.py`: 麦克风相关功能
- `Vocal.py`: 语音识别功能

## 功能特性

### 1. 核心音频处理 (Core.py)

#### 1.1 音频设备管理

- 列出所有音频设备
- 获取设备信息

#### 1.2 音频文件操作

- 保存音频为WAV格式
- 音频格式转换
- 播放音频文件
- 获取音频信息（时长、采样率、声道数、格式）

### 2. 麦克风功能 (Microphone.py)

#### 2.1 麦克风设备管理

- 列出所有麦克风设备
- 设备参数配置

#### 2.2 录音功能

- 定时录音
- 按键控制录音
- 流式录音
- 实时音频流处理

### 3. 语音识别 (Vocal.py)

#### 3.1 语音转文本

- 支持中文语音识别
- 支持多种音频格式
- 错误处理机制

## 使用示例

### 1. 音频处理示例

```python
from Convention.Audio import Core

# 播放音频文件
Core.play_audio("audio.wav")

# 录制音频
Core.record_audio("output.wav", duration=5)  # 录制5秒

# 转换音频格式
Core.convert_audio_format_with_file("input.wav", "output.mp3", "mp3")
```

### 2. 麦克风使用示例

```python
from Convention.Audio import Microphone

# 创建麦克风实例
mic = Microphone.light_microphone()

# 列出可用设备
devices = mic.list_microphone_devices()

# 录制音频
mic.recording_with_duration(5, "output.wav")  # 录制5秒

# 按键控制录音
mic.record_with_keypress("esc", "output.wav")  # 按ESC键停止录音
```

### 3. 语音识别示例

```python
from Convention.Audio import Vocal

# 创建语音识别实例
vocal = Vocal.light_vocal()

# 识别音频文件
text = vocal.audio_to_text("audio.wav")
print("识别结果:", text)
```

## 依赖项

- sounddevice: 音频设备控制
- soundfile: 音频文件处理
- numpy: 数值计算
- wave: WAV文件处理
- keyboard: 键盘控制
- SpeechRecognition: 语音识别

## 注意事项

1. 使用麦克风功能时需要确保有可用的录音设备
2. 语音识别功能需要网络连接
3. 音频格式转换可能需要安装额外的编解码器

## 性能优化

1. 对于长时间录音，建议使用流式处理
2. 语音识别时可以调整识别参数以提高准确率
3. 音频处理时注意采样率和位深的选择

## 贡献指南

欢迎提交Issue和Pull Request来改进功能或添加新特性。
