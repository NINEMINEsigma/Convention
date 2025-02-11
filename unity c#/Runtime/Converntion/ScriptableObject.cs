using System;
using System.Collections.Generic;
using UnityEngine;

namespace Convention
{
    [System.Serializable]
    public class SerializableDataDictionary : Dictionary<string, DataEntry>, ISerializationCallbackReceiver 
    {
        [Serializable]
        class Entry
        {
            public Entry() { }

            public Entry(string key, DataEntry value)
            {
                Key = key;
                Value = value;
            }

            public string Key;
            public DataEntry Value;
        }

        [SerializeField, Ignore]
        private List<Entry> Data;

        public virtual void OnBeforeSerialize()
        {
            Data = new();
            foreach (KeyValuePair<string, DataEntry> pair in this)
            {
                try
                {
                    Data.Add(new Entry(pair.Key, pair.Value ?? new DataEntry() { RealData = "" }));
                }
                catch { }
            }
        }

        // load dictionary from lists
        public virtual void OnAfterDeserialize()
        {
            if (Data == null) return;
            base.Clear();
            for (int i = 0; i < Data.Count; i++)
            {
                if (Data[i] != null)
                {
                    if (!base.TryAdd(Data[i].Key, Data[i].Value))
                    {
                        this.TryAdd($"{Data[i].Key}(new)", new() { RealData = "" });
                    }
                }
            }

            Data = null;
        }
    }
    [Serializable]
    public class DataEntry
#if UNITY_EDITOR
        : ISerializationCallbackReceiver
#endif
    {
        [SerializeField] public object RealData;

        public int intValue
        {
            get
            {
                if (ConventionUtility.IsNumber(RealData))
                    return (int)RealData;
                else if (ConventionUtility.IsString(RealData))
                    return ConventionUtility.convert_xvalue<int>((string)RealData);
                return 0;
            }
            set
            {
                if (ConventionUtility.IsString(RealData))
                    RealData = value.ToString();
                else
                    RealData = value;
            }
        }
        public long longValue
        {
            get
            {
                if (ConventionUtility.IsNumber(RealData))
                    return (long)RealData;
                else if (ConventionUtility.IsString(RealData))
                    return ConventionUtility.convert_xvalue<long>((string)RealData);
                return 0;
            }
            set
            {
                if (ConventionUtility.IsString(RealData))
                    RealData = value.ToString();
                else
                    RealData = value;
            }
        }
        public float floatValue
        {
            get
            {
                if (ConventionUtility.IsNumber(RealData))
                    return (float)RealData;
                else if (ConventionUtility.IsString(RealData))
                    return ConventionUtility.convert_xvalue<float>((string)RealData);
                return 0;
            }
            set
            {
                if (ConventionUtility.IsString(RealData))
                    RealData = value.ToString();
                else
                    RealData = value;
            }
        }
        public double doubleValue
        {
            get
            {
                if (ConventionUtility.IsNumber(RealData))
                    return (double)RealData;
                else if (ConventionUtility.IsString(RealData))
                    return ConventionUtility.convert_xvalue<double>((string)RealData);
                return 0;
            }
            set
            {
                if (ConventionUtility.IsString(RealData))
                    RealData = value.ToString();
                else
                    RealData = value;
            }
        }
        public string stringValue
        {
            get
            {
                if (ConventionUtility.IsNumber(RealData))
                    return ConventionUtility.convert_xstring(RealData);
                else if (ConventionUtility.IsString(RealData))
                    return (string)RealData;
                return RealData != null ? RealData.ToString() : "";
            }
            set
            {
                if (RealData != null)
                {
                    var pr = RealData.GetType().GetMethod("Parse", System.Reflection.BindingFlags.Static | System.Reflection.BindingFlags.Public);
                    if (pr != null)
                    {
                        RealData = pr.Invoke(RealData, new object[] { value });
                    }
                    else
                    {
                        RealData = value;
                    }
                }
                else
                    RealData = value;
            }
        }
        public bool boolValue
        {
            get
            {
                if (ConventionUtility.IsNumber(RealData))
                    return 0 != (double)RealData;
                else if (!ConventionUtility.IsString(RealData))
                {
                    string str = (string)RealData;
                    if ("false" == str || "0" == str)
                        return false;
                    else if ("true" == str || "1" == str)
                        return true;
                    else
                        throw new InvalidCastException($"\"{str}\" cannt cast to bool, only support \"true\" or \"1\", and \"false\" or \"0\"");
                }
                return (bool)RealData;
            }
            set
            {
                if (ConventionUtility.IsString(RealData))
                    RealData = value.ToString();
                else
                    RealData = value;
            }
        }
        public UnityEngine.Object uobjectValue
        {
            get
            {
                if (RealData != null && RealData.GetType().IsSubclassOf(typeof(UnityEngine.Object)))
                    return (UnityEngine.Object)RealData;
                return null;
            }
            set
            {
                RealData = value;
            }
        }
        public UnityEngine.MonoBehaviour monoBehaviour
        {
            get
            {
                if (RealData != null && RealData.GetType().IsSubclassOf(typeof(UnityEngine.MonoBehaviour)))
                    return (UnityEngine.MonoBehaviour)RealData;
                return null;
            }
            set
            {
                RealData = value;
            }
        }

#if UNITY_EDITOR
        public string CurrentValue;
        public UnityEngine.Object CurrentObject;
        public string RealType;

        public void OnAfterDeserialize()
        {
            if (CurrentObject != null) 
            {
                RealData = CurrentObject;
                return;
            }
            if (RealData == null)
            {
                if (int.TryParse(CurrentValue, out int iresult))
                    RealData = iresult;
                if (float.TryParse(CurrentValue, out float result))
                    RealData = result;
                else
                    RealData = CurrentValue;
                return;
            }
            var pr = RealData.GetType().GetMethod("Parse", System.Reflection.BindingFlags.Static | System.Reflection.BindingFlags.Public);
            if(pr!=null)
            {
                try
                {
                    var obj = pr.Invoke(null, new object[] { CurrentValue });
                    RealData = obj;
                }
                catch
                {
                }
                return;
            }
            RealData = CurrentValue;
        }

        public void OnBeforeSerialize()
        {
            CurrentValue = this.stringValue;
            CurrentObject = this.uobjectValue;
            RealType = RealData == null ? "null" : RealData.GetType().Name;
        }
#endif
    }

    [CreateAssetMenu(fileName = "new Convention", menuName = "Convention/Convention", order = -1)]
    [Serializable]
    public class ScriptableObject : UnityEngine.ScriptableObject, IAnyClass
    {
        [return: ReturnNotNull]
        public string SymbolName()
        {
            return "Convention." + nameof(ScriptableObject);
        }
#if UNITY_EDITOR
        public SerializableDataDictionary Datas;
#else
        public Dictionary<string, DataEntry> Datas;
#endif
    }
}
