using System.Collections.Generic;
using System.Linq;
using UnityEngine;

namespace Convention.WindowsUI.Variant
{
    public class FileSystemAssets : MonoSingleton<FileSystemAssets>
    {
        public static void InitLoadedRoots(ref List<string> LoadedInRoot)
        {
            LoadedInRoot = new List<string>();
            if (PlatformIndicator.is_platform_windows)
            {
                LoadedInRoot.Add(Application.persistentDataPath);
                LoadedInRoot.Add(Application.streamingAssetsPath);
            }
            else
            {
                LoadedInRoot.Add(Application.persistentDataPath);
            }
        }

        [Resources, SerializeField, OnlyNotNullMode] private AssetsWindow m_AssetsWindow;
        [Resources, OnlyNotNullMode] public Text CurrentSelectFilename;
        [Setting] public long LoadedFileAutoLoadMaxFileSize = 1024 * 50;
        [Setting] public List<string> LoadedInRoot = null;
        [Setting] public List<string> LoadedFileIconsExtension = new() { "skys", "scenes", "gameobjects" };
        [Setting] public string RootName = "Root";

        protected override void Awake()
        {
            base.Awake();
            // Starter
            FileSystemAssetsItem.LoadedFiles.Clear();
            if (LoadedInRoot == null || LoadedInRoot.Count == 0)
                InitLoadedRoots(ref LoadedInRoot);
            // Update extensions
            var extensions = ToolFile.AssetBundleExtension.ToList();
            extensions.AddRange(LoadedFileIconsExtension);
            ToolFile.AssetBundleExtension = extensions.ToArray();
            // Read Config
            if (LoadedFileAutoLoadMaxFileSize != 0)
                FileSystemAssetsItem.LoadedFileAutoLoadMaxFileSize = LoadedFileAutoLoadMaxFileSize;
        }

        private void Start()
        {
            var entries = m_AssetsWindow.MainPropertiesWindow.CreateRootItemEntries(false, LoadedInRoot.Count);
            for (int i = 0, e = LoadedInRoot.Count; i != e; i++)
            {
                entries[i].ref_value.GetComponent<FileSystemAssetsItem>().RebuildFileInfo(LoadedInRoot[i]);
            }
            m_AssetsWindow.Push(RootName, entries, true);
            CurrentSelectFilename.title = "";
        }

        private void Reset()
        {
            m_AssetsWindow = GetComponent<AssetsWindow>();
            InitLoadedRoots(ref LoadedInRoot);
            LoadedFileIconsExtension = new() { "skys", "scenes", "gameobjects" };
        }

        public void RefreshImmediate()
        {
            if (FocusWindowIndictaor.instance.Target == m_AssetsWindow.transform as RectTransform)
            {
                foreach (var entry in m_AssetsWindow.Peek())
                {
                    entry.ref_value.GetComponent<FileSystemAssetsItem>().SetDirty();
                }
            }
        }
    }
}