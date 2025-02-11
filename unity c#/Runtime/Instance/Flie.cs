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
#if UNITY_EDITOR
        , ISerializationCallbackReceiver
#endif
        {
            [Setting] public string FilePath;
            public void Refresh()
            {
                if(FilePath==null|| FilePath.Length==0)
                {
                    this.Datas.Clear();
                    return;
                }
                var file = new ToolFile(FilePath);
                if (file.Exists())
                {
                    this.Datas.TryAdd("data", new());
                    var data = this.Datas["data"].RealData = file.Load();
                    this.Datas.TryAdd("audio", new());
                    this.Datas["audio"].RealData = data.GetType() == typeof(AudioClip) ? data : null;
                    this.Datas.TryAdd("image", new());
                    this.Datas["image"].RealData = data.GetType() == typeof(Texture2D) ? data : null;
                    this.Datas.TryAdd("text", new());
                    this.Datas["text"].RealData = data.GetType() == typeof(string) ? data : null;
                }
                else
                {
                    this.Datas.Clear();
                }
            }

#if UNITY_EDITOR
            public bool is_refresh = true;
            public void OnAfterDeserialize()
            {

            }

            public void OnBeforeSerialize()
            {
                if (is_refresh)
                {
                    Refresh();
                    is_refresh = false;
                }
            }
#endif
        }
    }
}
