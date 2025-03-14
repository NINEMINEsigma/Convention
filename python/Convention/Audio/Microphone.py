import sounddevice  as     sd
import numpy        as     np
import wave
import keyboard
from ..File.Core      import tool_file

class light_microphone:
    def __init__(self, sample_rate=44100, channels=2):
        self.sample_rate = sample_rate
        self.channels = channels

    def list_microphone_devices(self):
        """列出所有麦克风设备"""
        return sd.query_devices()

    def recording_with_duration(self, duration:int, target:str=None):
        """录制音频并保存到文件"""
        frames = sd.rec(int(self.sample_rate * duration), samplerate=self.sample_rate, channels=self.channels, dtype='int16')
        sd.wait()
        if target:
            with wave.open(target, 'wb') as wf:
                wf.setnchannels(self.channels)
                wf.setsampwidth(2)  # 16-bit PCM
                wf.setframerate(self.sample_rate)
                wf.writeframes(frames.tobytes())
        return frames

    def stream_microphone(self, callback, duration=1000):
        """
        流式获取麦克风信号.
        持续时间，单位毫秒.
        """
        def _callback(indata, frames, time, status):
            if status:
                print(status, flush=True)
            callback(indata.copy())

        stream = sd.InputStream(samplerate=self.sample_rate, channels=self.channels, dtype='int16', callback=_callback)
        with stream:
            sd.sleep(duration)

    def start_recording(self):
        """开始录制音频"""
        self.is_recording = True
        self.frames = []
        def _callback(indata, frames, time, status):
            if status:
                print(status, flush=True)
            if self.is_recording:
                self.frames.append(indata.copy())

        self.stream = sd.InputStream(samplerate=self.sample_rate, channels=self.channels, dtype='int16', callback=_callback)
        self.stream.start()

    def stop_recording(self, target:str=None):
        """停止录制并保存音频到文件"""
        self.is_recording = False
        self.stream.stop()

        frames = np.concatenate(self.frames, axis=0)

        if target:
            with wave.open(".temp_"+target+".wav", 'wb') as wf:
                wf.setnchannels(self.channels)
                wf.setsampwidth(2)  # 16-bit PCM
                wf.setframerate(self.sample_rate)
                wf.writeframes(frames.tobytes())
            file = tool_file(".temp_"+target+".wav")
            file.load()
            file.save_as_audio(target)
            file.remove()
        return frames

    def record_with_keypress(self, end_key="esc", target:str=None):
        """开始录制，按e结束录制，并保存音频到文件"""
        self.start_recording()
        keyboard.add_hotkey(end_key, self.stop_recording, args=(target,))
        keyboard.wait(end_key)

# 示例使用
if __name__ == "__main__":
    mic = light_microphone()

    # 列出麦克风设备
    devices = mic.list_microphone_devices()
    print("麦克风设备列表:", devices)

    # 录制并保存音频到文件
    mic.record_with_keypress(target="output.mp3")
