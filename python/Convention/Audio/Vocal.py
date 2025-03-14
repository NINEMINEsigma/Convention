import speech_recognition as sr

class light_vocal:
    def __init__(self):
        self.recognizer = sr.Recognizer()

    def audio_to_text(self, audio_file_path:str):
        """
        将音频文件转换为文本。
        
        参数:
        audio_file_path (str): 音频文件的路径。
        
        返回:
        str: 识别出的文本。
        """
        try:
            with sr.AudioFile(audio_file_path) as source:
                audio_data = self.recognizer.record(source)
                text = self.recognizer.recognize_google(audio_data, language='zh-CN')
                return text
        except sr.UnknownValueError:
            return "无法理解音频"
        except sr.RequestError as e:
            return f"请求出错; {e}"

# 使用示例
if __name__ == "__main__":
    lv = light_vocal()
    result = lv.audio_to_text("path_to_your_audio_file.wav")
    print("识别结果:", result)
