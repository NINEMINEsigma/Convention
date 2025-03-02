using System.Collections;
using System.Collections.Generic;
using System.IO;
using Convention.SO;
using UnityEditor;
using UnityEngine;

namespace Convention
{
    [CustomEditor(typeof(ToolFileSO))]
    public class FileEditor : AbstractCustomEditor
    {
        [MenuItem("Convention/AssetBundle/Create for Android")]
        static void CreatAssetBundle()
        {
            string path = Path.Combine(Application.streamingAssetsPath, "AssetBundle", "Android");
            if (!Directory.Exists(path))
            {
                Directory.CreateDirectory(path);
            }
            BuildPipeline.BuildAssetBundles(path, BuildAssetBundleOptions.None, BuildTarget.Android);
            UnityEngine.Debug.Log("Android Finish!");
        }

        [MenuItem("Convention/AssetBundle/Create for IOS")]
        static void BuildAllAssetBundlesForIOS()
        {
            string dirName = "AssetBundles/IOS/IOS";
            if (!Directory.Exists(dirName))
            {
                Directory.CreateDirectory(dirName);
            }
            BuildPipeline.BuildAssetBundles(dirName, BuildAssetBundleOptions.None, BuildTarget.iOS);
            UnityEngine.Debug.Log("IOS Finish!");

        }

        [MenuItem("Convention/AssetBundle/Create for Windows")]
        static void CreatPCAssetBundleForwINDOWS()
        {
            string path = Path.Combine(Application.streamingAssetsPath, "AssetBundle", "Window");
            if (!Directory.Exists(path))
            {
                Directory.CreateDirectory(path);
            }
            BuildPipeline.BuildAssetBundles(path, BuildAssetBundleOptions.None, BuildTarget.StandaloneWindows64);
            UnityEngine.Debug.Log("Windows Finish!");
        }

        [MenuItem("Convention/AssetBundle/Create for Mac")]
        static void CreatPCAssetBundleForMac()
        {
            string path = Path.Combine(Application.streamingAssetsPath, "AssetBundle", "Mac");
            if (!Directory.Exists(path))
            {
                Directory.CreateDirectory(path);
            }
            BuildPipeline.BuildAssetBundles(path, BuildAssetBundleOptions.None, BuildTarget.StandaloneOSX);
            UnityEngine.Debug.Log("Mac Finish!");
        }
    }
}
