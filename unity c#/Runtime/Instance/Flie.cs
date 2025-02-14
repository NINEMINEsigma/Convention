using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Convention
{
    namespace SO
    {
        [CreateAssetMenu(fileName = "new ToolFile", menuName = "Convention/ToolFile", order = 100)]
        [Serializable]
        public class ToolFileSO : ScriptableObject
        {
            [Setting, ToolFile.File, TextArea(2, 5)] public string FilePath;
            [OnlyPlayMode, Content, Ignore] public ToolFile File;
            public void Refresh()
            {
                if (Application.isPlaying == false)
                    throw new Exception("Just refresh on play mode");
                if (FilePath == null || FilePath.Length == 0)
                {
                    this.Datas.Clear();
                    return;
                }
                File = new ToolFile(FilePath);
                if (File.Exists())
                {
                    this.Datas.TryAdd("data", new());
                    var data = this.Datas["data"].RealData = File.Load();
                    this.Datas.TryAdd("audio", new());
                    this.Datas["audio"].RealData = data.GetType() == typeof(AudioClip) ? data : null;
                    this.Datas.TryAdd("clip", new());
                    this.Datas["clip"].RealData = data.GetType() == typeof(AudioClip) ? data : null;
                    this.Datas.TryAdd("image", new());
                    this.Datas["image"].RealData = data.GetType() == typeof(Texture2D) ? data : null;
                    this.Datas.TryAdd("texture", new());
                    this.Datas["texture"].RealData = data.GetType() == typeof(Texture2D) ? data : null;
                    this.Datas.TryAdd("text", new());
                    this.Datas["text"].RealData = data.GetType() == typeof(string) ? data : null;
                }
                else
                {
                    this.Datas.Clear();
                }
            }

        }
    }
}
