using System;
using System.Collections;
using System.Collections.Generic;
using ES3Internal;
using UnityEngine;

namespace Convention
{
    public interface IES3 { }
    public static class ES3Plugin
    {
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

        public static void Save(this IES3 self, string path) => Save(path, self);
        public static void Save(this IES3 self, string key, string path) => Save(key, self, path);
        public static void QuickSave(this object self, string path) => Save(path, self);
        public static void QuickSave(this object self, string key, string path) => Save(key, self, path);
    }
}

