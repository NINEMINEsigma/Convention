using UnityEngine;
using UnityEngine.Networking;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;
using Newtonsoft;
using Newtonsoft.Json;

namespace Convention
{
    [Serializable]
    public class ToolURL : LeftValueReference<string>
#if UNITY_EDITOR
        , ISerializationCallbackReceiver
#endif
    {
        [System.AttributeUsage(AttributeTargets.All, Inherited = true, AllowMultiple = false)]
        public class URLAttribute : Attribute
        {
            public string[] urlTypes;
            public URLAttribute(params string[] types)
            {
                urlTypes = types;
            }
            public URLAttribute(bool IsAnyURL)
            {
                if (IsAnyURL)
                    urlTypes = new string[] { "*" };
                else
                    urlTypes = new string[] { };
            }
        }

        public static string[] ImageURLTypes = new string[] { "jpg", "jpeg", "png", "gif", "bmp", "webp" };
        public static string[] AudioURLTypes = new string[] { "mp3", "wav", "ogg", "aac", "m4a" };
        public static string[] VideoURLTypes = new string[] { "mp4", "webm", "mov", "avi", "mkv" };
        public static string[] DocumentURLTypes = new string[] { "pdf", "doc", "docx", "txt", "rtf" };
        public static string[] JsonURLTypes = new string[] { "json" };

        [Content, SerializeField] private string url { get => ref_value; set => ref_value = value; }
        [Ignore][HideInInspector] public UnityWebRequest WebRequest { get; protected set; }
        [Content] public object data;

        public ToolURL([In] string url) : base(url) { }

        ~ToolURL()
        {
            this.Close();
        }

        public override string SymbolName()
        {
            return
                $"url<" +
                $"{(IsValid ? "v" : "-")}" +
                $">";
        }

        public override string ToString()
        {
            return this.url;
        }

        public delegate void GetCallback([In] UnityWebRequest request);
        public bool Get([In] GetCallback callback)
        {
            if (!IsValid)
                return false;

            WebRequest = UnityWebRequest.Get(this.url);
            WebRequest.SendWebRequest();

            while (!WebRequest.isDone) ;

            callback(WebRequest);
            return WebRequest.result == UnityWebRequest.Result.Success;
        }

        public delegate void GetAsyncCallback([In, Opt] UnityWebRequest request);
        public IEnumerator GetAsync([In] GetAsyncCallback callback)
        {
            if (!IsValid)
            {
                callback(null);
                yield break;
            }

            WebRequest = UnityWebRequest.Get(this.url);
            WebRequest.SendWebRequest();

            while (!WebRequest.isDone) 
                yield return null;

            callback(WebRequest);
        }

        public delegate void PostCallback([In] UnityWebRequest request);
        public delegate UnityWebRequest PostIniter([In]UnityWebRequest request);
        public bool Post([In] PostCallback callback, [In]WWWForm form)
        {
            if (!IsValid)
                return false;

            WebRequest = UnityWebRequest.Post(this.url, form);
            WebRequest.SendWebRequest();

            while (!WebRequest.isDone) ;

            callback(WebRequest);
            return WebRequest.result == UnityWebRequest.Result.Success;
        }
        public bool Post([In]PostCallback callback, [In]PostIniter initer,  [In]WWWForm form)
        {
            if (!IsValid)
                return false;

            WebRequest = initer(UnityWebRequest.Post(this.url, form));
            WebRequest.SendWebRequest();

            while (!WebRequest.isDone) ;

            callback(WebRequest);
            return WebRequest.result == UnityWebRequest.Result.Success;
        }

        public delegate void PostAsyncCallback([In, Opt] UnityWebRequest request);
        public IEnumerator PostAsync([In] PostAsyncCallback callback, [In] WWWForm form)
        {
            if (!IsValid)
            {
                callback(null);
                yield break;
            }

            WebRequest = UnityWebRequest.Post(this.url, form);
            WebRequest.SendWebRequest();

            while (!WebRequest.isDone)
                yield return null;

            callback(WebRequest);
        }

        #region URL Properties
        public string FullURL => this.url;
        public static implicit operator string(ToolURL data) => data.FullURL;
        public string GetFullURL()
        {
            return this.url;
        }

        public string GetFilename()
        {
            if (string.IsNullOrEmpty(this.url))
                return "";

            Uri uri = new Uri(this.url);
            string path = uri.AbsolutePath;
            return Path.GetFileName(path);
        }

        public string GetExtension()
        {
            string filename = GetFilename();
            if (string.IsNullOrEmpty(filename))
                return "";

            return Path.GetExtension(filename);
        }

        public bool ExtensionIs(params string[] extensions)
        {
            string el = GetExtension().ToLower();
            string eln = el.Length > 1 ? el[1..] : null;
            foreach (string extension in extensions)
                if (el == extension || eln == extension)
                    return true;
            return false;
        }
        #endregion

        #region Validation
        public bool IsValid => ValidateURL();
        public bool ValidateURL()
        {
            if (string.IsNullOrEmpty(this.url))
                return false;

            return Uri.TryCreate(this.url, UriKind.Absolute, out Uri uriResult)
                && (uriResult.Scheme == Uri.UriSchemeHttp || uriResult.Scheme == Uri.UriSchemeHttps);
        }
        public static implicit operator bool(ToolURL url) => url.IsValid;
        #endregion

        public ToolURL Refresh()
        {
            this.Close();
            return this;
        }

        public static object LoadObjectFromBinary([In] byte[] data)
        {
            using MemoryStream fs = new(data, false);
            return new System.Runtime.Serialization.Formatters.Binary.BinaryFormatter().Deserialize(fs);
        }

        public ToolURL Data2Object()
        {
            this.data = LoadObjectFromBinary((byte[])this.data);
            return this;
        }

        public ToolURL Data2Bytes()
        {
            using MemoryStream ms = new();
            new System.Runtime.Serialization.Formatters.Binary.BinaryFormatter().Serialize(ms, this.data);
            var outdata = ms.GetBuffer();
            this.data = outdata;
            return this;
        }

        public bool IsDataByteArray()
        {
            return this.data != null && this.data.GetType() == typeof(byte[]);
        }

        #region Load
        public object Load()
        {
            if (IsText)
                return this.LoadAsText();
            else if (IsJson)
                return LoadAsJson();
            else if (IsImage)
                return LoadAsImage();
            else if (IsAudio)
                return LoadAsAudio();
            else if (IsDocument)
                return LoadAsDocument();
            else
                return LoadAsText();
        }

        public static T LoadFromText<T>([In] string requestText)
        {
            return JsonConvert.DeserializeObject<T>(requestText);
        }
        [return: ReturnMayNull]
        public static T LoadFromRequest<T>([In] UnityWebRequest request)
        {
            while (!request.isDone) ;

            return request.result == UnityWebRequest.Result.Success
                ? LoadFromText<T>(request.downloadHandler.text)
                : default;
        }
        [return: ReturnNotNull]
        public static T LoadFromRequestNotNull<T>([In] UnityWebRequest request) where T : class, new()
        {
            while (!request.isDone) ;

            return request.result == UnityWebRequest.Result.Success
                ? LoadFromText<T>(request.downloadHandler.text)
                : new();
        }

        public object LoadAsJson()
        {
            return LoadAsJsonWithType<object>();
        }

        public T LoadAsJsonWithType<T>()
        {
            string jsonText = LoadAsText();
            try
            {
                T result = JsonConvert.DeserializeObject<T>(jsonText);
                this.data = result;
                return result;
            }
            catch (Exception)
            {
                Debug.Log(jsonText);
                return default;
            }
        }

        public T LoadAsJsonWithType<T>(T whenThrow)
        {
            string jsonText = LoadAsText();
            try
            {
                T result = JsonConvert.DeserializeObject<T>(jsonText);
                this.data = result;
                return result;
            }
            catch (Exception)
            {
                Debug.Log(jsonText);
                return whenThrow;
            }
        }

        public string LoadAsText()
        {
            if (!IsValid)
                return null;

            WebRequest = UnityWebRequest.Get(this.url);
            WebRequest.SendWebRequest();

            while (!WebRequest.isDone)
            {
                // 等待请求完成
            }

            if (WebRequest.result == UnityWebRequest.Result.Success)
            {
                this.data = WebRequest.downloadHandler.text;
                return (string)this.data;
            }

            return null;
        }

        public IEnumerator LoadAsTextAsync([In] Action<string> callback)
        {
            if (!IsValid)
            {
                callback(null);
                yield break;
            }

            WebRequest = UnityWebRequest.Get(this.url);
            yield return WebRequest.SendWebRequest();

            if (WebRequest.result == UnityWebRequest.Result.Success)
            {
                this.data = WebRequest.downloadHandler.text;
                callback((string)this.data);
            }
            else
            {
                callback(null);
            }
        }

        public byte[] LoadAsBinary()
        {
            if (!IsValid)
                return null;

            WebRequest = UnityWebRequest.Get(this.url);
            WebRequest.SendWebRequest();

            while (!WebRequest.isDone)
            {
                // 等待请求完成
            }

            if (WebRequest.result == UnityWebRequest.Result.Success)
            {
                this.data = WebRequest.downloadHandler.data;
                return (byte[])this.data;
            }

            return null;
        }

        public IEnumerator LoadAsBinaryAsync([In] Action<byte[]> callback)
        {
            if (!IsValid)
            {
                callback(null);
                yield break;
            }

            WebRequest = UnityWebRequest.Get(this.url);
            yield return WebRequest.SendWebRequest();

            if (WebRequest.result == UnityWebRequest.Result.Success)
            {
                this.data = WebRequest.downloadHandler.data;
                callback((byte[])this.data);
            }
            else
            {
                callback(null);
            }
        }

        public Texture2D LoadAsImage()
        {
            if (!IsValid)
                return null;

            WebRequest = UnityWebRequestTexture.GetTexture(this.url);
            WebRequest.SendWebRequest();

            while (!WebRequest.isDone)
            {
                // 等待请求完成
            }

            if (WebRequest.result == UnityWebRequest.Result.Success)
            {
                this.data = DownloadHandlerTexture.GetContent(WebRequest);
                return (Texture2D)this.data;
            }

            return null;
        }

        public IEnumerator LoadAsImageAsync([In] Action<Texture2D> callback)
        {
            if (!IsValid)
            {
                callback(null);
                yield break;
            }

            WebRequest = UnityWebRequestTexture.GetTexture(this.url);
            yield return WebRequest.SendWebRequest();

            if (WebRequest.result == UnityWebRequest.Result.Success)
            {
                this.data = DownloadHandlerTexture.GetContent(WebRequest);
                callback((Texture2D)this.data);
            }
            else
            {
                callback(null);
            }
        }

        public AudioClip LoadAsAudio()
        {
            if (!IsValid)
                return null;

            WebRequest = UnityWebRequestMultimedia.GetAudioClip(this.url, GetAudioType());
            WebRequest.SendWebRequest();

            while (!WebRequest.isDone)
            {
                // 等待请求完成
            }

            if (WebRequest.result == UnityWebRequest.Result.Success)
            {
                this.data = DownloadHandlerAudioClip.GetContent(WebRequest);
                return (AudioClip)this.data;
            }

            return null;
        }

        public IEnumerator LoadAsAudioAsync([In] Action<AudioClip> callback)
        {
            if (!IsValid)
            {
                callback(null);
                yield break;
            }

            WebRequest = UnityWebRequestMultimedia.GetAudioClip(this.url, GetAudioType());
            yield return WebRequest.SendWebRequest();

            if (WebRequest.result == UnityWebRequest.Result.Success)
            {
                this.data = DownloadHandlerAudioClip.GetContent(WebRequest);
                callback((AudioClip)this.data);
            }
            else
            {
                callback(null);
            }
        }

        public byte[] LoadAsDocument()
        {
            return LoadAsBinary();
        }

        public IEnumerator LoadAsDocumentAsync([In] Action<byte[]> callback)
        {
            yield return LoadAsBinaryAsync(callback);
        }

        public object LoadAsUnknown()
        {
            this.data = this.LoadAsBinary();
            return this;
        }
        #endregion

        #region Save
        public void Save([In][Opt] string localPath = null)
        {
            if (IsText)
                SaveAsText(localPath);
            else if (IsJson)
                SaveAsJson(localPath);
            else if (IsImage)
                SaveAsImage(localPath);
            else if (IsAudio)
                SaveAsAudio(localPath);
            else if (IsVideo)
                SaveAsVideo(localPath);
            else if (IsDocument)
                SaveAsDocument(localPath);
            else
                SaveAsBinary(localPath);
        }

        public void SaveAsJson([In][Opt] string localPath = null)
        {
            if (localPath == null)
            {
                localPath = Path.Combine(Application.temporaryCachePath, GetFilename());
            }

            string jsonText = JsonConvert.SerializeObject(this.data);
            File.WriteAllText(localPath, jsonText);
        }

        public void SaveAsText([In][Opt] string localPath = null)
        {
            if (localPath == null)
            {
                localPath = Path.Combine(Application.temporaryCachePath, GetFilename());
            }

            File.WriteAllText(localPath, (string)this.data);
        }

        public void SaveAsBinary([In][Opt] string localPath = null)
        {
            if (localPath == null)
            {
                localPath = Path.Combine(Application.temporaryCachePath, GetFilename());
            }

            File.WriteAllBytes(localPath, (byte[])this.data);
        }

        public void SaveAsImage([In][Opt] string localPath = null)
        {
            if (localPath == null)
            {
                localPath = Path.Combine(Application.temporaryCachePath, GetFilename());
            }

            Texture2D texture = (Texture2D)this.data;
            byte[] bytes = texture.EncodeToPNG();
            File.WriteAllBytes(localPath, bytes);
        }

        public void SaveAsAudio([In][Opt] string localPath = null)
        {
            // 注意：Unity不直接支持将AudioClip保存为文件
            // 这里只是一个示例，实际应用中可能需要其他方法
            Debug.LogWarning("Direct saving of AudioClip to file is not supported in Unity");
        }

        public void SaveAsVideo([In][Opt] string localPath = null)
        {
            // 注意：Unity不直接支持将VideoClip保存为文件
            // 这里只是一个示例，实际应用中可能需要其他方法
            Debug.LogWarning("Direct saving of VideoClip to file is not supported in Unity");
        }

        public void SaveAsDocument([In][Opt] string localPath = null)
        {
            SaveAsBinary(localPath);
        }
        #endregion

        public static AudioType GetAudioType(string url)
        {
            return Path.GetExtension(url) switch
            {
                "ogg" => AudioType.OGGVORBIS,
                "mp2" => AudioType.MPEG,
                "mp3" => AudioType.MPEG,
                "mod" => AudioType.MOD,
                "wav" => AudioType.WAV,
                "aif" => AudioType.IT,
                _ => AudioType.UNKNOWN
            };
        }

        public AudioType GetAudioType()
        {
            return GetAudioType(this.url);
        }

        #region URL Types
        public bool IsText => ExtensionIs("txt", "html", "htm", "css", "js", "xml", "csv");
        public bool IsJson => ExtensionIs(JsonURLTypes);
        public bool IsImage => ExtensionIs(ImageURLTypes);
        public bool IsAudio => ExtensionIs(AudioURLTypes);
        public bool IsVideo => ExtensionIs(VideoURLTypes);
        public bool IsDocument => ExtensionIs(DocumentURLTypes);
        #endregion

        #region Operators
        [return: ReturnNotNull, ReturnNotSelf]
        public static ToolURL operator |([In] ToolURL left, [In] string rightPath)
        {
            string baseUrl = left.GetFullURL();
            if (baseUrl.EndsWith("/"))
            {
                return new ToolURL(baseUrl + rightPath);
            }
            else
            {
                return new ToolURL(baseUrl + "/" + rightPath);
            }
        }

        public ToolURL Open([In] string url)
        {
            this.url = url;
            return this;
        }

        public ToolURL Close()
        {
            if (WebRequest != null)
            {
                WebRequest.Dispose();
                WebRequest = null;
            }
            return this;
        }

        public ToolURL Download([In][Opt] string localPath = null)
        {
            if (!IsValid)
                return this;

            if (localPath == null)
            {
                localPath = Path.Combine(Application.temporaryCachePath, GetFilename());
            }

            WebRequest = UnityWebRequest.Get(this.url);
            WebRequest.SendWebRequest();

            while (!WebRequest.isDone)
            {
                // 等待请求完成
            }

            if (WebRequest.result == UnityWebRequest.Result.Success)
            {
                File.WriteAllBytes(localPath, WebRequest.downloadHandler.data);
            }

            return this;
        }

        public IEnumerator DownloadAsync([In] Action<bool> callback, [In][Opt] string localPath = null)
        {
            if (!IsValid)
            {
                callback(false);
                yield break;
            }

            if (localPath == null)
            {
                localPath = Path.Combine(Application.temporaryCachePath, GetFilename());
            }

            WebRequest = UnityWebRequest.Get(this.url);
            yield return WebRequest.SendWebRequest();

            if (WebRequest.result == UnityWebRequest.Result.Success)
            {
                File.WriteAllBytes(localPath, WebRequest.downloadHandler.data);
                callback(true);
            }
            else
            {
                callback(false);
            }
        }
        #endregion

#if UNITY_EDITOR
        [SerializeField, ToolURL.URL] private string m_URL;
        public void OnBeforeSerialize()
        {
            m_URL = this.FullURL;
        }

        public void OnAfterDeserialize()
        {
            if (m_URL != this.FullURL)
            {
                this.url = m_URL;
            }
        }
#endif
    }
}
