using UnityEngine;

namespace Convention
{
    public interface IES3 { }
    public static class ES3Plugin
    {
        #region Base Save

        public static void Save<T>(string path, T data)
        {
            Save("easy", data, path);
        }
        public static void Save(string path, object data)
        {
            Save("easy", data, path);
        }
        public static void Save<T>(string key, T data, string path)
        {
            ES3.Save(key, data, path);
        }
        public static void Save(string key, object data, string path)
        {
            ES3.Save(key, data, path);
        }

        #endregion

        #region Base Load

        public static T Load<T>(string path)
        {
            return Load<T>("easy", path);
        }
        public static object Load(string path)
        {
            return ES3.Load("easy", path);
        }
        public static T Load<T>(string key, string path)
        {
            return ES3.Load<T>(key, path);
        }
        public static object Load(string key, string path)
        {
            return ES3.Load(key, path);
        }

        #endregion

        public static void Save(this IES3 self, string path) => Save<object>(path, self);
        public static void Save(this IES3 self, string key, string path) => Save<object>(key, self, path);
        public static void QuickSave(this object self, string path) => Save(path, self);
        public static void QuickSave(this object self, string key, string path) => Save(key, self, path);

        #region Other Load

        public static byte[] LoadRawBytes(string filePath) => ES3.LoadRawBytes(filePath);
        public static string LoadRawString(string filePath) => ES3.LoadRawString(filePath);
        public static Texture2D LoadImage(string imagePath) => ES3.LoadImage(imagePath);
        public static Texture2D LoadImage(byte[] bytes) => ES3.LoadImage(bytes);
        public static AudioClip LoadAudio(string audioFilePath
#if UNITY_2018_3_OR_NEWER
                                        , AudioType audioType
                                    ) => ES3.LoadAudio(audioFilePath, audioType);
#else
                                    )=>ES3.LoadAudio(audioFilePath);
#endif

        #endregion

        public static Texture2D ConvertTexture2D(byte[] bytes) => ES3.LoadImage(bytes);

        #region Serialize/Deserialize

        public static byte[] Serialize<T>(T value) => ES3.Serialize(value);
        public static T Deserialize<T>(byte[] bytes) => ES3.Deserialize<T>(bytes);
        public static void DeserializeInto<T>(byte[] bytes, T obj) where T : class => ES3.DeserializeInto(bytes, obj);

        #endregion

        public static byte[] Serialize(this IES3 self) => Serialize<object>(self);
        public static T DeserializeFrom<T>(this T self, byte[] bytes) where T : class, IES3
        {
            DeserializeInto(bytes, self);
            return self;
        }

        #region Encrypt/Decrypt

        public static byte[] EncryptBytes(byte[] bytes, string password = null) => ES3.EncryptBytes(bytes, password);

        public static byte[] DecryptBytes(byte[] bytes, string password = null) => ES3.DecryptBytes(bytes, password);

        public static string EncryptString(string str, string password = null) => ES3.EncryptString(str, password);

        public static string DecryptString(string str, string password = null) => ES3.DecryptString(str, password);

        public static byte[] CompressBytes(byte[] bytes) => ES3.CompressBytes(bytes);

        public static byte[] DecompressBytes(byte[] bytes) => ES3.DecompressBytes(bytes);

        public static string CompressString(string str) => ES3.CompressString(str);

        public static string DecompressString(string str) => ES3.DecompressString(str);

        #endregion

        public static void InitExtensionEnv()
        {
            ES3.Init();
        }
    }
}

