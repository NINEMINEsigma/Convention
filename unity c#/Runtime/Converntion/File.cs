using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Serialization.Formatters.Binary;
using UnityEngine;
using UnityEngine.Networking;
using UnityEditor;

namespace Convention
{
    [Serializable]
    public class ToolFile : LeftValueReference<FileInfo>
#if UNITY_EDITOR
        , ISerializationCallbackReceiver
#endif
    {
        [System.AttributeUsage(AttributeTargets.All, Inherited = true, AllowMultiple = false)]
        public class FileAttribute : Attribute
        {
            public string[] fileTypes;
            public FileAttribute(params string[] extensions)
            {
                fileTypes = extensions;
            }
            public FileAttribute(bool IsAnyFile)
            {
                if (IsAnyFile)
                    fileTypes = new string[] { "*" };
                else
                    fileTypes = new string[] { };
            }
        }
        public static string[] TextFileExtensions = new string[] { "txt", "ini" };
        public static string[] AudioFileExtension = new string[] { "ogg", "mp2", "mp3", "mod", "wav", "it" };
        public static string[] ImageFileExtension = new string[] { "png", "jpg", "jpeg", "bmp", "tif", "icon" };
        public static string[] AssetBundleExtension = new string[] { "AssetBundle", "AssetBundle".ToLower(), "ab" };

        public static AudioType GetAudioType(string path)
        {
            return Path.GetExtension(path) switch
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

        [Ignore][HideInInspector] public FileStream Stream { get; protected set; }
        [Content] public object data;

        public ToolFile([In] string path) : base(new FileInfo(path)) { }
        public ToolFile([In] FileInfo fileInfo) : base(fileInfo) { }
        ~ToolFile()
        {
            this.Close();
        }
        public override string SymbolName()
        {
            return
                $"file<" +
                $"{(IsExist ? "e" : "-")}" +
                $">";
        }

        #region Path
        public string FullPath => this.ref_value == null ? "" : this.ref_value.FullName;
        public static implicit operator string(ToolFile data) => data.FullPath;
        public string GetFullPath()
        {
            return this.ref_value.FullName;
        }
        public string Filename => this.ref_value.Name;
        public string GetFilename(bool is_ignore_extension = false)
        {
            string result = this.ref_value.Name;
            if (result.Contains('.') && is_ignore_extension)
            {
                result = result[..result.LastIndexOf('.')];
            }
            else if (result[^1] == '/' || result[^1] == '\\')
            {
                result = result[..^1];
            }
            return result;
        }
        public string Extension => this.ref_value.Extension;
        public string GetExtension()
        {
            return this.ref_value.Extension;
        }
        public bool ExtensionIs(params string[] extensions)
        {
            string el = Extension.ToLower();
            foreach (string extension in extensions)
                if (el == extension.ToLower() || el[1..] == extension.ToLower())
                    return true;
            return false;
        }
        #endregion
        #region Exists
        public bool IsExist => this.ref_value.Exists;
        public bool Exists() => this.ref_value.Exists;
        public static implicit operator bool(ToolFile file) => file.IsExist;
        #endregion

        public ToolFile Refresh(bool isRefreshData = false)
        {
            this.ref_value.Refresh();
            if (isRefreshData)
            {
                this.Load();
            }
            return this;
        }

        public static object LoadObjectFromBinary([In] byte[] data)
        {
            using MemoryStream fs = new(data, false);
            return new BinaryFormatter().Deserialize(fs);
        }
        public ToolFile Data2Object()
        {
            this.data = LoadObjectFromBinary((byte[])this.data);
            return this;
        }
        public ToolFile Data2Bytes()
        {
            using MemoryStream ms = new();
            new BinaryFormatter().Serialize(ms, this.data);
            var outdata = ms.GetBuffer();
            this.data = outdata;
            return this;
        }
        public bool IsDataByteArray()
        {
            return this.data.GetType() == typeof(byte[]);
        }

        #region Load
        public object Load()
        {
            if (this.ExtensionIs(TextFileExtensions))
                return this.LoadAsText();
            else if (this.ExtensionIs("json"))
                return LoadAsJson();
            else if (this.ExtensionIs(ImageFileExtension))
                return LoadAsImage();
            else if (this.ExtensionIs(AudioFileExtension))
                return LoadAsAudio();
            else if (this.ExtensionIs(AssetBundleExtension))
                return LoadAsAssetBundle();
            else if (IsBinaryFile())
                return LoadAsBinary();
            else
                return LoadAsText();
        }
        public object LoadAsJson()
        {
            this.data = ES3Plugin.Load(FullPath);
            return this.data;
        }
        public string LoadAsText()
        {
            string result = "";
            using (var fs = this.ref_value.OpenText())
            {
                result = fs.ReadToEnd();
            }
            this.data = result;
            return result;
        }
        public byte[] LoadAsBinary()
        {
            const int BlockSize = 1024;
            long FileSize = this.ref_value.Length;
            byte[] result = new byte[FileSize];
            long offset = 0;
            using (var fs = this.ref_value.OpenRead())
            {
                fs.ReadAsync(result[(int)(offset)..(int)(offset + BlockSize)], 0, BlockSize);
                offset += BlockSize;
                offset = System.Math.Min(offset, FileSize);
            }
            this.data = result;
            return result;
        }
        public Texture2D LoadAsImage()
        {
            //string path = FullPath;
            //UnityWebRequest request = UnityWebRequestTexture.GetTexture(path);
            //request.SendWebRequest().MarkCompleted(() =>
            //{
            //    if (request.result == UnityWebRequest.Result.Success)
            //    {
            //        this.data = DownloadHandlerTexture.GetContent(request);
            //    }
            //    else this.data = null;
            //});
            //return this.data as Texture2D;
            return ES3Plugin.LoadImage(FullPath);
        }
        public IEnumerator LoadAsImage([In] Action callback)
        {
            UnityWebRequest request = UnityWebRequestTexture.GetTexture(FullPath);
            yield return request.SendWebRequest();

            if (request.result == UnityWebRequest.Result.Success)
            {
                this.data = DownloadHandlerTexture.GetContent(request);
            }
            callback();
        }
        public AudioClip LoadAsAudio()
        {
            //UnityWebRequest request = UnityWebRequestMultimedia.GetAudioClip(FullPath, GetAudioType(FullPath));
            //request.SendWebRequest().MarkCompleted(() =>
            //{
            //    if (request.result == UnityWebRequest.Result.Success)
            //    {
            //        this.data = DownloadHandlerAudioClip.GetContent(request);
            //    }
            //    else this.data = null;
            //});
            //return this.data as AudioClip;
            return ES3Plugin.LoadAudio(FullPath, GetAudioType(FullPath));
        }
        public IEnumerator LoadAsAudio([In] Action callback)
        {
            UnityWebRequest request = UnityWebRequestMultimedia.GetAudioClip(FullPath, GetAudioType(FullPath));
            yield return request.SendWebRequest();

            if (request.result == UnityWebRequest.Result.Success)
            {
                this.data = DownloadHandlerAudioClip.GetContent(request);
            }
            else this.data = null;
            callback();
        }
        public AssetBundle LoadAsAssetBundle()
        {
            var result = AssetBundle.LoadFromFile(FullPath);
            this.data = result;
            return result;
        }
        public object LoadAsUnknown()
        {
            this.data = this.LoadAsBinary();
            return this;
        }
        #endregion
        #region Save
        public void Save([In][Opt] string newpath = null)
        {
            if (this.ExtensionIs(TextFileExtensions))
                SaveAsText(newpath);
            else if (this.ExtensionIs("json"))
                SaveAsJson(newpath);
            else if (this.ExtensionIs(ImageFileExtension))
                SaveAsImage(newpath);
            else if (this.ExtensionIs(AudioFileExtension))
                SaveAsAudio(newpath);
            else if (this.ExtensionIs(AssetBundleExtension))
                SaveAsAssetBundle(newpath);
            else if (IsBinaryFile() == false)
                SaveAsText(newpath);
            SaveAsBinary(newpath);
        }
        public void SaveAsJson([In][Opt] string newpath = null)
        {
            ES3Plugin.Save(newpath ?? FullPath, this.data);
        }
        public void SaveAsText([In][Opt] string newpath = null)
        {
            if (Stream != null && Stream.CanWrite)
            {
                using var sw = new StreamWriter(Stream);
                sw.Write((string)this.data);
                sw.Flush();
            }
            else
            {
                using var fs = new FileStream(newpath ?? FullPath, FileMode.CreateNew, FileAccess.Write);
                using var sw = new StreamWriter(fs);
                sw.Write((string)this.data);
                sw.Flush();
            }
        }
        public static void SaveDataAsBinary([In] string path, [In] byte[] outdata, [In][Opt] FileStream Stream = null)
        {
            if (Stream != null && Stream.CanWrite)
            {
                Stream.Write(outdata, 0, outdata.Length);
                Stream.Flush();
            }
            else
            {
                using var fs = new FileStream(path, FileMode.CreateNew, FileAccess.Write);
                fs.Write(outdata, 0, outdata.Length);
                fs.Flush();
            }
        }
        public void SaveAsImage([In][Opt] string newpath = null)
        {
            Texture2D texture = (Texture2D)this.data;
            byte[] bytes = texture.EncodeToPNG();
            SaveDataAsBinary(newpath ?? FullPath, bytes, Stream);
        }
        public void SaveAsAudio([In][Opt] string newpath = null)
        {
            throw new NotImplementedException();
        }
        public void SaveAsAssetBundle([In][Opt] string newpath = null)
        {
            throw new NotImplementedException();
        }
        public void SaveAsBinary([In][Opt] string newpath = null)
        {
            using MemoryStream ms = new();
            new BinaryFormatter().Serialize(ms, this.data);
            SaveDataAsBinary(newpath ?? FullPath, ms.GetBuffer(), Stream);
        }
        #endregion

        public static bool IsBinaryFile([In] string path, float p = 0.7f)
        {
            int read_length = 1024;

            using (var fs = File.OpenRead(path))
            {
                byte[] bytes = new byte[read_length];
                int length = fs.Read(bytes, 0, bytes.Length);

                int textchar = 0;
                foreach (byte b in bytes)
                {
                    if ((b > 0x20 && b < 0x7F) || b == '\n' || b == '\r' || b == '\t')
                        textchar++;
                }
                //属于字符类型的字节大于某个比例后认为这是一个文本文件
                //由于可能是宽字符所以置信度需要自行操作
                if (length * p < textchar)
                    return false;
            }

            return true;
        }
        [return: ReturnNotNull] public static string GetExtension([In] string path) => new FileInfo(path).Extension;
        [return: ReturnNotNull] public static string GetBaseFilename([In] string path) => Path.GetFileName(path);

        public long FileSize => this.ref_value.Length;

        #region IsFileType
        public bool IsDir()
        {
            if (Exists())
            {
                return Directory.Exists(this.ref_value.FullName);
            }
            return this.ref_value.FullName[^1] == '\\' || this.ref_value.FullName[^1] == '/';
        }
        public bool IsFile()
        {
            if (Exists())
            {
                return File.Exists(this.ref_value.FullName);
            }
            return this.ref_value.FullName[^1] != '\\' && this.ref_value.FullName[^1] != '/';
        }
        public bool IsFileEmpty()
        {
            return this.ref_value.Length == 0;
        }
        public bool IsBinaryFile()
        {
            return IsBinaryFile(this.GetFullPath());
        }
        #endregion
        #region Operator
        [return: ReturnNotNull, ReturnNotSelf]
        public static ToolFile operator |([In] ToolFile left, [In] string rightPath)
        {
            string lp = left.GetFullPath();
            return new ToolFile(Path.Combine(lp, rightPath));
        }
        public ToolFile Open([In] string path)
        {
            this.ref_value = new FileInfo(path);
            return this;
        }
        public ToolFile Open([In] FileInfo fileInfo)
        {
            this.ref_value = fileInfo;
            return this;
        }
        public ToolFile Open([In][Opt] FileMode? mode)
        {
            if (mode != null)
            {
                this.Close();
                Stream = new FileStream(this.ref_value.FullName, mode.Value);
            }
            return this;
        }
        public ToolFile Close()
        {
            if (Stream != null)
            {
                Stream.Close();
            }
            return this;
        }
        public ToolFile Create()
        {
            if (Exists() == false)
            {
                if (IsDir())
                    Directory.CreateDirectory(this.ref_value.FullName);
                else
                    File.Create(this.ref_value.FullName);
            }
            return this;
        }
        [return: ReturnSelf]
        public ToolFile Rename([In] string filename_or_path)
        {
            if (filename_or_path.Contains('\\') || filename_or_path.Contains('/'))
            {
                filename_or_path = filename_or_path[filename_or_path.LastIndexOf('\\', '/')..];
            }
            this.ref_value.MoveTo(Path.Join(this.ref_value.Directory.FullName, filename_or_path));
            return this;
        }
        [return: ReturnSelf]
        public ToolFile Move([In] string path)
        {
            this.ref_value.MoveTo(path);
            return this;
        }
        [return: ReturnNotSelf]
        public ToolFile Copy([In] string path)
        {
            return new(this.ref_value.CopyTo(path));
        }
        [return: ReturnNotSelf]
        public ToolFile Copy([In] string path, bool overwrite)
        {
            return new(this.ref_value.CopyTo(path, overwrite));
        }
        [return: ReturnSelf]
        public ToolFile Delete()
        {
            if (IsDir())
                Directory.Delete(FullPath);
            else
                File.Delete(FullPath);
            return this;
        }
        public ToolFile MustExistPath()
        {
            this.Close();
            this.TryCreateParentPath();
            this.Create();
            return this;
        }
        #endregion
        #region Dir
        public ToolFile TryCreateParentPath()
        {
            if (File.Exists(this.ref_value.DirectoryName) == false)
            {
                Directory.CreateDirectory(this.ref_value.DirectoryName);
            }
            return this;
        }
        [return: ReturnMayNull, When("this.IsDir()==false")]
        public IEnumerable<FileInfo> DirIter()
        {
            if (this.IsDir())
            {
                return new DirectoryInfo(FullPath).EnumerateFiles();
            }
            return null;
        }
        [return: ReturnNotNull]
        public List<ToolFile> DirToolFileIter()
        {
            if (this.IsDir())
            {
                return new DirectoryInfo(FullPath).EnumerateFiles().ToList().ConvertAll(x => new ToolFile(x));
            }
            throw new DirectoryNotFoundException(FullPath);
        }
        [return: ReturnSelf]
        public ToolFile BackToParentDir()
        {
            this.ref_value = new FileInfo(this.ref_value.DirectoryName);
            return this;
        }
        [return: ReturnNotSelf]
        public ToolFile GetParentDir()
        {
            return new ToolFile(this.ref_value.DirectoryName);
        }
        [return: NotSucceed(-1)]
        public int DirCount()
        {
            if (IsDir())
                return Directory.EnumerateFiles(FullPath).Count();
            return -1;
        }
        [return: ReturnSelf]
        public ToolFile DirClear()
        {
            if (IsDir())
            {
                foreach (var file in DirIter())
                {
                    file.Delete();
                }
            }
            throw new DirectoryNotFoundException();
        }
        [return: ReturnSelf]
        public ToolFile MakeFileInside(string source, bool isDeleteSource = false)
        {
            if (this.IsDir() == false)
                throw new DirectoryNotFoundException(FullPath);
            string target = this | source;
            if (isDeleteSource)
                File.Move(target, source);
            else
                File.Copy(target, source);
            return this;
        }
        #endregion

#if UNITY_EDITOR
        [SerializeField, ToolFile.File] private string m_Path;
        public void OnBeforeSerialize()
        {
            m_Path = this.FullPath;
        }

        public void OnAfterDeserialize()
        {
            if (m_Path != this.FullPath)
            {
                this.ref_value = new(m_Path);
            }
        }
#endif


#if UNITY_SWITCH
        public static readonly string persistentDataPath = "";
        public static readonly string dataPath = "";
#else
        public static string persistentDataPath => Application.persistentDataPath;
        public static string dataPath => Application.dataPath;
#endif
        public const string backupFileSuffix = ".bac";
        public const string temporaryFileSuffix = ".tmp";

        public static DateTime GetTimestamp(string filePath)
        {
            return File.GetLastWriteTime(filePath).ToUniversalTime();
        }

        public static string BrowseFile([In] params string[] extensions)
        {
            string result = null;
#if PLATFORM_STANDALONE_WIN
            {
                WindowsKit.SelectFileOnSystem((string file) =>
                {
                    result = file;
                }, "Browse", "File", extensions);
            }
#endif
            return result;
        }
        public static ToolFile BrowseToolFile([In] params string[] extensions)
        {
            return new ToolFile(BrowseFile(extensions));
        }
    }
}
