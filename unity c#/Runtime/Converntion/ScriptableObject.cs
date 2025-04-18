using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering;

namespace Convention
{
    [CreateAssetMenu(fileName = "new Convention", menuName = "Convention/Convention", order = -1)]
    [Serializable, ArgPackage]
    public class ScriptableObject : UnityEngine.ScriptableObject, IAnyClass
    {
        [return: ReturnNotNull]
        public string SymbolName()
        {
            return "Convention." + nameof(ScriptableObject);
        }

        public SerializedDictionary<string, UnityEngine.Object> uobjects = new();
        public SerializedDictionary<string, string> symbols = new();
        public SerializedDictionary<string, float> values = new();

        public T FindItem<T>(string key, T defaultValue = default)
        {
            var typen = typeof(T);
            if (typen.IsSubclassOf(typeof(UnityEngine.Object)))
            {
                if (uobjects.TryGetValue(key, out var uobj) && uobj is T uobj_r)
                    return uobj_r;
            }
            else if (typen.IsSubclassOf(typeof(string)))
            {
                if (symbols.TryGetValue(key, out var str) && str is T str_r)
                    return str_r;
            }
            else if (typen.IsSubclassOf(typeof(float)))
            {
                if (values.TryGetValue(key, out var fvalue) && fvalue is T fvalue_r)
                    return fvalue_r;
            }
            else if (typen.IsSubclassOf(typeof(int)))
            {
                if (values.TryGetValue(key, out var ivalue) && ((int)ivalue) is T ivalue_r)
                    return ivalue_r;
            }
            else if (typen.IsSubclassOf(typeof(bool)))
            {
                if (values.TryGetValue(key, out var bvalue) && (bvalue != 0) is T bvalue_r)
                    return bvalue_r;
            }
            return defaultValue;
        }

        public virtual void Reset()
        {
            uobjects.Clear();
            values.Clear();
            symbols.Clear();
        }
    }
}
