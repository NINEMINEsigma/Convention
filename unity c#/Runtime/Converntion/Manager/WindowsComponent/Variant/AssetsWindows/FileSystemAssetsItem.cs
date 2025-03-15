using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

namespace Convention.WindowsUI.Variant
{
    public class FileSystemAssetsItem : MonoAnyBehaviour, AssetsItem.IAssetsItemInvoke
    {
        public static Dictionary<string, ToolFile> LoadedFiles = new();
        public static long LoadedFileAutoLoadMaxFileSize = 1024 * 50;
        [Setting] public ToolFile m_File;
        [Setting, InspectorDraw(InspectorDrawType.Text), Ignore] public string ItemPathName;
        [Setting] public ScriptableObject m_Icons;
        [Content, OnlyPlayMode, SerializeField] private bool m_Dirty = false;
        [Content, OnlyNotNullMode, SerializeField, InspectorDraw(InspectorDrawType.Toggle), Ignore]
        private bool m_IsLoading = false;

        private void OnDestroy()
        {
            if (m_File.data is AssetBundle)
            {
                return;
            }
            m_File.data = null;
        }

        public void RebuildFileInfo([In] string path)
        {
            if (LoadedFiles.ContainsKey(path))
                m_File = LoadedFiles[path];
            else
            {
                LoadedFiles.Add(path, new ToolFile(path));
                m_File = new(path);
            }
            m_Dirty = true;
            ItemPathName = m_File.GetFilename(false);
            OnAssetsItemFocusWithFileMode(GetComponent<AssetsItem>(), m_File.GetFilename(true));
        }
        public void RebuildButNotFileInfo([In] string title)
        {
            var temp = new ToolFile(title);
            ItemPathName = temp.GetFilename(false);
            if (title.Contains("."))
                UpdateSpriteAndTitle(GetComponent<AssetsItem>(), title, title.Split('.')[^1]);
            else
                UpdateSpriteAndTitle(GetComponent<AssetsItem>(), title, "ab.data");
        }

        private void UpdateSprite([In] AssetsItem item, [In] string sprite)
        {
            item.ButtonSprite = m_Icons.Datas[sprite].uobjectValue as Sprite;
        }

        private void UpdateSpriteAndTitle([In] AssetsItem item, [In] string name, [In] string extension)
        {
            item.title = name;
            UpdateSprite(item, extension);
        }
        /// <summary>
        /// Some extensions see <see cref="FileSystemAssets.Awake"/>
        /// </summary>
        /// <param name="item"></param>
        private void OnAssetsItemFocusWithFileMode([In] AssetsItem item, [In] string name)
        {
            item.title = name;
            FileSystemAssets.instance.CurrentSelectFilename.title = m_File.FullPath;
            if (m_File.IsExist == false)
                return;
            else if (m_File.IsDir())
                UpdateSprite(item, "folder");
            else if (m_File.Extension.Length != 0 && m_Icons.Datas.ContainsKey(m_File.Extension))
                UpdateSprite(item, m_File.Extension);
            else if (m_File.Extension.Length != 0 && m_Icons.Datas.ContainsKey(m_File.Extension[1..]))
                UpdateSprite(item, m_File.Extension[1..]);
            else if (m_File.IsImage)
                UpdateSprite(item, "image");
            else if (m_File.IsText)
                UpdateSprite(item, "text");
            else if (m_File.IsJson)
                UpdateSprite(item, "json");
            else if (m_File.IsAssetBundle)
                UpdateSprite(item, "ab");
            else
                UpdateSprite(item, "default");
        }

        // --------------------

        private abstract class AssetBundleItem : WindowUIModule, AssetsItem.IAssetsItemInvoke
        {
            [Resources] public AssetBundle ab;
            [Resources] public string targetName;

            public abstract void OnAssetsItemFocus(AssetsItem item);
            public abstract void OnAssetsItemInvoke(AssetsItem item);
        }
        private class SkyItem : AssetBundleItem
        {
            [Resources, SerializeField] private Material SkyBox;
            public class SkyItemInstanceWrapper : Singleton<SkyItemInstanceWrapper>
            {
                public static void InitInstance()
                {
                    if (instance == null)
                    {
                        instance = new SkyItemInstanceWrapper();
                    }
                }
                [InspectorDraw(InspectorDrawType.Reference), Ignore] public Material SkyBox;
                [InspectorDraw(InspectorDrawType.Text), Ignore] public string SkyBoxName;
            }

            private void OnEnable()
            {
                SkyItemInstanceWrapper.InitInstance();
            }
            private void OnDisable()
            {
                SkyBox = null;
            }

            public override void OnAssetsItemFocus(AssetsItem item)
            {
                if (SkyBox == null)
                {
                    SkyBox = ab.LoadAsset<Material>(targetName);
                }
            }

            public override void OnAssetsItemInvoke(AssetsItem item)
            {
                SkyExtension.Load(SkyBox);
                if (!HierarchyWindow.instance.ContainsReference(SkyItemInstanceWrapper.instance))
                {
                    var skyItem = HierarchyWindow.instance.CreateRootItemEntryWithBinders(SkyItemInstanceWrapper.instance)[0].ref_value.GetComponent<HierarchyItem>();
                    skyItem.title = "Global Skybox";
                }
                SkyItemInstanceWrapper.instance.SkyBox = SkyBox;
                SkyItemInstanceWrapper.instance.SkyBoxName = targetName;
                InspectorWindow.instance.ClearWindow();
            }
        }
        private class SceneItem : AssetBundleItem
        {
            [Resources, SerializeField] private bool isLoad = false;
            [Content, SerializeField, Ignore] private Scene m_Scene;
            private PropertiesWindow.ItemEntry m_entry;

            public override void OnAssetsItemFocus(AssetsItem item)
            {
            }

            public override void OnAssetsItemInvoke(AssetsItem item)
            {
                if (isLoad = !isLoad)
                {
                    SceneExtension.Load(targetName);
                    item.GetComponent<Image>().color = Color.green;
                    // init scene item
                    m_Scene = SceneExtension.GetScene(targetName);
                    m_entry = HierarchyWindow.instance.CreateRootItemEntryWithBinders(item)[0];
                    var hierarchyItem = m_entry.ref_value.GetComponent<HierarchyItem>();
                    hierarchyItem.title = m_Scene.name;
                }
                else
                {
                    SceneExtension.Unload(targetName);
                    item.GetComponent<Image>().color = Color.white;
                    // unload scene item
                    m_entry.Release();
                }
            }
        }
        private class GameObjectItem : AssetBundleItem, IOnlyFocusThisOnInspector
        {
            [Content, IsInstantiated(true)] public GameObject target;
            [Content, IsInstantiated(true)] public GameObject last;
            private Image image;

            [InspectorDraw(InspectorDrawType.Button), Content]
            public void ReleaseGameObjectToScene()
            {
                if (target != null)
                {
                    HierarchyWindow.instance.CreateRootItemEntryWithGameObject(target);
                    target = null;
                }
            }
            [InspectorDraw(InspectorDrawType.Button), Content]
            public void DestroyCurrentSelect()
            {
                if (target != null)
                {
                    if (HierarchyWindow.instance.ContainsReference(target))
                    {
                        HierarchyWindow.instance.GetReferenceItem(target).Entry.Release();
                        HierarchyWindow.instance.RemoveReference(target);
                    }
                    GameObject.Destroy(target);
                    target = null;
                }
            }


            private void OnEnable()
            {
                image = GetComponent<Image>();
            }
            private void OnDisable()
            {
                if (target != null)
                {
                    GameObject.Destroy(target);
                }
            }

            private void FixedUpdate()
            {
                image.color = target == null ? Color.white : Color.green;
            }

            public override void OnAssetsItemFocus(AssetsItem item)
            {
            }

            public override void OnAssetsItemInvoke(AssetsItem item)
            {
                if (target == null)
                {
                    last = target;
                    target = GameObject.Instantiate(ab.LoadAsset<GameObject>(targetName));
                    try
                    {
                        target.BroadcastMessage($"On{nameof(GameObjectItem)}", this, SendMessageOptions.DontRequireReceiver);
                    }
                    catch (InvalidOperationException)
                    {
                        target = null;
                    }
                }
                else
                {
                    ReleaseGameObjectToScene();
                }
            }
        }

        // --------------------

        private void OnAssetsItemFocusWithFileLoading([In] AssetsItem item)
        {
            if (m_File.IsAssetBundle)
            {
                if (m_File.data == null)
                {
                    StartLoad();
                    StartCoroutine(this.m_File.LoadAsAssetBundle(LoadAssetBundle));
                }
            }
            else
            {
                if (m_File.data == null && m_File.FileSize < 1024 * 50)
                    m_File.Load();

            }

            void StartLoad()
            {
                m_IsLoading = true;
                item.GetComponent<Image>().color = Color.red;
            }
            void EndLoad()
            {
                m_IsLoading = false;
                item.GetComponent<Image>().color = Color.white;
            }
            void LoadAssetBundle()
            {
                EndLoad();
                AssetBundle ab = m_File.data as AssetBundle;
                var assets = ab.GetAllAssetNames().ToList();
                assets.AddRange(ab.GetAllScenePaths());
                var entries = item.AddChilds(assets.Count);
                for (int i = 0, e = assets.Count; i < e; i++)
                {
                    entries[i].ref_value.GetComponent<FileSystemAssetsItem>().RebuildButNotFileInfo(assets[i]);
                    AssetBundleItem abitem = null;
                    var objectName = new ToolFile(assets[i]).GetFilename(false);
                    if (objectName.Contains("is sky") || m_File.ExtensionIs("skys"))
                    {
                        abitem = entries[i].ref_value.gameObject.AddComponent<SkyItem>();
                    }
                    else if (m_File.ExtensionIs("scenes"))
                    {
                        abitem = entries[i].ref_value.gameObject.AddComponent<SceneItem>();
                        UpdateSpriteAndTitle(entries[i].ref_value.gameObject.GetComponent<AssetsItem>(), new ToolFile(assets[i]).GetFilename(true), "scene");
                        abitem.targetName = assets[i];
                    }
                    else if (objectName.Contains("is gameobject") || m_File.ExtensionIs("gameobjects"))
                    {
                        abitem = entries[i].ref_value.gameObject.AddComponent<GameObjectItem>();
                    }
                    if (abitem != null)
                    {
                        if (abitem.ab == null)
                            abitem.ab = ab;
                        if (abitem.targetName == null || abitem.targetName.Length == 0)
                            abitem.targetName = objectName;
                    }
                }
            }
        }
        private void RefreshWithToolFile([In] AssetsItem item)
        {
            ReleaseAllChilds(item);
            if (m_File.IsDir())
            {
                var files = m_File.DirToolFileIter();
                var paths = (from file in files
                             where !file.ExtensionIs("meta")
                             where !file.Filename.ToLower().Contains("<ignore file>")
                             select file).ToList().ConvertAll(x => x.FullPath);
                AddChils(item, paths);
            }
            else
            {
                OnAssetsItemFocusWithFileLoading(item);
            }
            ItemPathName = m_File.GetFilename(false);
            OnAssetsItemFocusWithFileMode(item, m_File.GetFilename(true));
        }

        private void ReleaseAllChilds(AssetsItem item)
        {
            item.RemoveAllChilds();
        }
        private List<PropertiesWindow.ItemEntry> AddChils([In] AssetsItem item, [In] List<string> paths)
        {
            var entries = item.AddChilds(paths.Count);
            for (int i = 0, e = paths.Count; i < e; i++)
            {
                entries[i].ref_value.GetComponent<FileSystemAssetsItem>().RebuildFileInfo(paths[i]);
            }
            return entries;
        }

        public void OnAssetsItemFocus([In] AssetsItem item)
        {
            if (m_Dirty)
            {
                if (m_File != null)
                {
                    RefreshWithToolFile(item);
                }
                m_Dirty = false;
            }
            item.HasChildLayer = item.ChildCount() != 0 ||
                ((m_File != null && m_File.IsExist) && (m_File.IsDir() || m_File.IsAssetBundle));
            FileSystemAssets.instance.CurrentSelectFilename.title = ItemPathName;
            InspectorWindow.instance.SetTarget(this, null);
        }

        public void OnAssetsItemInvoke([In] AssetsItem item)
        {
            //while (m_IsLoading)
            {
#if UNITY_EDITOR
                //    Debug.Log("file loading", this);
#endif
            }
        }

        [Content, OnlyPlayMode]
        public void SetDirty()
        {
            m_Dirty = true;
        }
    }
}
