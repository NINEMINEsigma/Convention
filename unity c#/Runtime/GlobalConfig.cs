using System.Collections;
using System.Collections.Generic;
using Newtonsoft.Json;
using UnityEngine;

namespace Convention
{
    public class GlobalConfig : AnyClass
    {
        public static string ConstConfigFile = "config.json";
        public static void InitExtensionEnv()
        {
            ConstConfigFile = "config.json";
        }

        public static void GenerateEmptyConfigJson([In]ToolFile file)
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

        public GlobalConfig([In, Opt] ToolFile dataDir, bool isTryCreateDataDir = false, bool isLoad = true)
        {
            if (dataDir == null)
                dataDir = new ToolFile(".");
            this.DataDir = dataDir;
            if(this.DataDir.IsDir()==false)
                this.DataDir.BackToParentDir();
        }

        public ToolFile GetConfigFile() => DataDir | ConstConfigFile;
    }
}
