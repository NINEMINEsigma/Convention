using System;
using System.Collections;
using System.Collections.Generic;
using Newtonsoft.Json;
using UnityEngine;

namespace Convention
{
    public class GlobalConfig : AnyClass, IEnumerable<KeyValuePair<string,object>>
    {
        public static string ConstConfigFile = "config.json";
        public static void InitExtensionEnv()
        {
            ConstConfigFile = "config.json";
        }

        public static void GenerateEmptyConfigJson([In] ToolFile file)
        {
            file.Open(System.IO.FileMode.CreateNew);
            file.data = JsonConvert.SerializeObject(new Dictionary<string, object>()
            {
                { "properties",new Dictionary<string, object>() }
            });
            file.Save();
        }

        [SerializeField, Setting] private int configLogging_tspace = "Property not found".Length;

        private ToolFile DataDir;
        private Dictionary<string, object> data_pair = new();

        public GlobalConfig([In, Opt] string dataDir, bool isTryCreateDataDir = false, bool isLoad = true) 
            : this(new ToolFile(dataDir), isTryCreateDataDir, isLoad) { }
        public GlobalConfig([In, Opt] ToolFile dataDir, bool isTryCreateDataDir = false, bool isLoad = true)
        {
            // build up data folder
            dataDir ??= new ToolFile("./");
            this.DataDir = dataDir;
            if (this.DataDir.IsDir() == false)
                this.DataDir.BackToParentDir();
            if (this.DataDir.Exists()==false)
            {
                if(isTryCreateDataDir)
                    this.DataDir.MustExistsPath();
                else
                    throw new Exception($"Data dir not found: {this.DataDir}");
            }
            // build up init data file
            var configFile = this.ConfigFile;
            if (configFile.Exists() == false)
                GenerateEmptyConfigJson(configFile);
            else if (isLoad)
                this.LoadProperties();
        }
        ~GlobalConfig()
        {

        }

        public ToolFile GetConfigFile() => DataDir | ConstConfigFile;
        public ToolFile ConfigFile => GetConfigFile();

        public ToolFile GetFile([In] string path, bool isMustExist = false)
        {
            var file = DataDir | path;
            if (isMustExist)
                file.MustExistsPath();
            return file;
        }
        public bool EraseFile([In] string path)
        {
            var file = DataDir | path;
            if (file.Exists())
            {
                try
                {
                    file.Open(System.IO.FileMode.Create);
                    return true;
                }
                catch (Exception) { }
            }
            return false;
        }
        public bool RemoveFile([In] string path)
        {
            var file = DataDir | path;
            if (file.Exists())
            {
                try
                {
                    file.Delete();
                    return true;
                }
                catch (Exception) { }
            }
            return false;
        }
        public bool CreateFile([In] string path)
        {
            var file = DataDir | path;
            if (file.Exists())
                return false;
            if (file.GetParentDir().Exists() == false)
                return false;
            file.Create();
            return true;
        }

        public object this[[In]string key]
        {
            get
            {
                return data_pair[key];
            }
            set
            {
                data_pair[key] = value;
            }
        }
        public bool Contains([In] string key) => data_pair.ContainsKey(key);
        public bool Remove([In] string key)
        {
            if (data_pair.ContainsKey(key))
            {
                data_pair.Remove(key);
                return true;
            }
            return false;
        }
        public IEnumerator<KeyValuePair<string, object>> GetEnumerator()
        {
            return ((IEnumerable<KeyValuePair<string, object>>)this.data_pair).GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return ((IEnumerable)this.data_pair).GetEnumerator();
        }
        public int DataSize() => data_pair.Count;

        public GlobalConfig SaveProperties()
        {
            var configFile = this.ConfigFile;
            configFile.Open(System.IO.FileMode.Create);
            configFile.data = JsonConvert.SerializeObject(new Dictionary<string, Dictionary<string, object>>
            {
                {"properties", data_pair }
            });
            configFile.Save();
            return this;
        }
        public GlobalConfig LoadProperties()
        {
            var configFile = this.ConfigFile;
            if(configFile.Exists()==false)
            {
                data_pair = new();
            }
            else 
            {
                var data = configFile.LoadAsRawJson<Dictionary<string, Dictionary<string, object>>>();
                if (data.ContainsKey("properties"))
                {
                    data_pair = data["properties"];
                }
                else
                {
                    throw new Exception($"Can't find properties not found in config file");
                }
            }
            return this;
        }

        public ToolFile GetLogFile()
        {
            return this.GetFile(ConfigFile.GetFilename(true) + "_log.txt", true);
        }
        public ToolFile LogFile => GetLogFile();
        public virtual void Log(string messageType, string message, string color)
        {
            configLogging_tspace = Mathf.Max(configLogging_tspace, messageType.Length);
            var what =$"{}: {message}"
        }
    }
}
