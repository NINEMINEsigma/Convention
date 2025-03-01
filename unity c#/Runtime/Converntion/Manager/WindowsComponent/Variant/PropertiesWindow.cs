using System;
using System.Collections;
using System.Collections.Generic;
using System.Numerics;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.UI;
using static Convention.WindowsUI.Variant.PropertiesWindow;

namespace Convention.WindowsUI.Variant
{
    public class PropertiesWindow : MonoAnyBehaviour
    {
        [Serializable, ArgPackage]
        public class ItemEntry : LeftValueReference<WindowUIModule>
        {
            #region Tools

            private static bool IsSetupParentRectTransformAdjustSizeToContainsChilds = false;

            private static void SetupParentRectTransform([In] RectTransform child, [In] WindowManager parent)
            {
                parent.AddContextChild(child, IsSetupParentRectTransformAdjustSizeToContainsChilds);
            }
            private static void SetupParentRectTransform([In] RectTransform child, [In] RectTransform parent)
            {
                if (parent.GetComponent<WindowManager>() != null)
                    SetupParentRectTransform(child, parent.GetComponent<WindowManager>());
                else if (parent.GetComponents<BaseWindowPlane>().Length != 0)
                    parent.GetComponents<BaseWindowPlane>()[0].AddChild(child, IsSetupParentRectTransformAdjustSizeToContainsChilds);
                else
                    RectTransformExtension.SetParentAndResize(parent, child, IsSetupParentRectTransformAdjustSizeToContainsChilds);
            }

            [return: IsInstantiated(true)]
            public static WindowUIModule InstantiateItemObject([In] string prefab, [In] WindowManager windowManager, [In] SO.Windows config)
            {
                var go = Instantiate(config.GetWindowsUI<WindowUIModule>(prefab).gameObject);
                var rgo = go.GetComponent<RectTransform>();
                SetupParentRectTransform(rgo, windowManager);
                return go.GetComponent<WindowUIModule>();
            }
            [return: IsInstantiated(true)]
            public static WindowUIModule InstantiateItemObject([In] string prefab, [In] RectTransform plane, [In] SO.Windows config)
            {
                var go = Instantiate(config.GetWindowsUI<WindowUIModule>(prefab).gameObject);
                var rgo = go.GetComponent<RectTransform>();
                SetupParentRectTransform(rgo, plane);
                return go.GetComponent<WindowUIModule>();
            }
            [return: IsInstantiated(true)]
            public static WindowUIModule InstantiateItemObject(
                [In, IsInstantiated(false)] WindowUIModule prefab,
                [In] WindowManager windowManager)
            {
                var go = Instantiate(prefab.gameObject);
                var rgo = go.GetComponent<RectTransform>();
                SetupParentRectTransform(rgo, windowManager);
                return go.GetComponent<WindowUIModule>();
            }
            [return: IsInstantiated(true)]
            public static WindowUIModule InstantiateItemObject(
                [In, IsInstantiated(false)] WindowUIModule prefab,
                [In] RectTransform plane)
            {
                var go = Instantiate(prefab.gameObject);
                var rgo = go.GetComponent<RectTransform>();
                SetupParentRectTransform(rgo, plane);
                return go.GetComponent<WindowUIModule>();
            }

            [return: ArgPackage]
            public static ItemEntry MakeItemWithInstantiate([In] string prefab, [In] PropertiesWindow parent)
            {
                return new ItemEntry(parent)
                {
                    ref_value = InstantiateItemObject(prefab, parent.m_WindowManager, parent.m_WindowsConfig)
                };
            }
            [return: ArgPackage]
            public static ItemEntry MakeItemWithInstantiate([In] string prefab, [In] ItemEntry parent, [In] SO.Windows config)
            {
                return new ItemEntry(parent)
                {
                    ref_value = InstantiateItemObject(prefab, parent.ref_value.GetComponent<RectTransform>(), config)
                };
            }
            [return: ArgPackage]
            public static ItemEntry MakeItemWithInstantiate(
                [In, IsInstantiated(false)] WindowUIModule prefab,
                [In] PropertiesWindow parent)
            {
                return new ItemEntry(parent)
                {
                    ref_value = InstantiateItemObject(prefab, parent.m_WindowManager)
                };
            }
            [return: ArgPackage]
            public static ItemEntry MakeItemWithInstantiate(
                [In, IsInstantiated(false)] WindowUIModule prefab,
                [In] ItemEntry parent)
            {
                return new ItemEntry(parent)
                {
                    ref_value = InstantiateItemObject(prefab, parent.ref_value.GetComponent<RectTransform>())
                };
            }

            [return: ArgPackage]
            public static ItemEntry MakeItem([In] PropertiesWindow parent)
            {
                return new ItemEntry(parent);
            }
            [return: ArgPackage]
            public static ItemEntry MakeItem([In] ItemEntry parent)
            {
                return new ItemEntry(parent);
            }

            public static IActionInvoke MakeItemAsActionInvoke(
                [In, Out] ItemEntry entry,
                [In] string invokerName, [In] PropertiesWindow parent,
                [In][Opt, When("If you sure not need a target")] IAnyClass target,
                params UnityAction<IAnyClass>[] actions)
            {
                entry.ref_value = InstantiateItemObject(invokerName, parent.m_WindowManager, parent.m_WindowsConfig);
                var invoker = entry.ref_value as IActionInvoke;
                foreach (var action in actions)
                {
                    invoker.AddListener(() => action.Invoke(target));
                }
                return invoker;
            }
            public static IButton MakeItemAsActionInvoke(
                [In, Out] ItemEntry entry,
                [In] string invokerName, [In] ItemEntry parent, [In] SO.Windows config,
                [In][Opt, When("If you sure not need a target")] IAnyClass target,
                params UnityAction<IAnyClass>[] actions)
            {
                entry.ref_value = InstantiateItemObject(invokerName, parent.ref_value.GetComponent<RectTransform>(), config);
                var invoker = entry.ref_value as IButton;
                foreach (var action in actions)
                {
                    invoker.AddListener(() => action.Invoke(target));
                }
                return invoker;
            }

            public static IButton MakeItemAsButton(
                [In, Out] ItemEntry entry,
                [In] string buttonName, [In] PropertiesWindow parent,
                [In][Opt, When("If you sure not need a target")] IAnyClass target,
                params UnityAction<IAnyClass>[] actions)
            {
                entry.ref_value = InstantiateItemObject(buttonName, parent.m_WindowManager, parent.m_WindowsConfig);
                var button = entry.ref_value as IButton;
                foreach (var action in actions)
                {
                    button.AddListener(() => action.Invoke(target));
                }
                return button;
            }
            public static IButton MakeItemAsButton(
                [In, Out] ItemEntry entry,
                [In] string buttonName, [In] ItemEntry parent, [In] SO.Windows config,
                [In][Opt, When("If you sure not need a target")] IAnyClass target,
                params UnityAction<IAnyClass>[] actions)
            {
                entry.ref_value = InstantiateItemObject(buttonName, parent.ref_value.GetComponent<RectTransform>(), config);
                var button = entry.ref_value as IButton;
                foreach (var action in actions)
                {
                    button.AddListener(() => action.Invoke(target));
                }
                return button;
            }

            public static IText MakeItemAsText(
                [In, Out] ItemEntry entry,
                [In] string textName, [In] PropertiesWindow parent,
                [In] string textStr)
            {
                entry.ref_value = InstantiateItemObject(textName, parent.m_WindowManager, parent.m_WindowsConfig);
                var text = entry.ref_value as IText;
                text.text = textStr;
                return text;

            }
            public static IText MakeItemAsText(
                [In, Out] ItemEntry entry,
                [In] string textName, [In] ItemEntry parent, [In] SO.Windows config,
                [In] string textStr)
            {
                entry.ref_value = InstantiateItemObject(textName, parent.ref_value.GetComponent<RectTransform>(), config);
                var text = entry.ref_value as IText;
                text.text = textStr;
                return text;
            }

            public static ITitle MakeItemAsTitle(
                [In, Out] ItemEntry entry,
                [In] string textName, [In] PropertiesWindow parent,
                [In] string textStr)
            {
                entry.ref_value = InstantiateItemObject(textName, parent.m_WindowManager, parent.m_WindowsConfig);
                var text = entry.ref_value as ITitle;
                text.title = textStr;
                return text;

            }
            public static ITitle MakeItemAsTitle(
                [In, Out] ItemEntry entry,
                [In] string textName, [In] ItemEntry parent, [In] SO.Windows config,
                [In] string textStr)
            {
                entry.ref_value = InstantiateItemObject(textName, parent.ref_value.GetComponent<RectTransform>(), config);
                var text = entry.ref_value as ITitle;
                text.title = textStr;
                return text;
            }

            #endregion

            public override WindowUIModule ref_value
            {
                get => base.ref_value;
                set
                {
                    if (base.ref_value != value)
                    {
                        if (base.ref_value != null)
                        {
                            base.ref_value.gameObject.SetActive(false);
                        }
                        base.ref_value = value;
                        if (parentWindow != null)
                        {
                            //parentWindow.m_WindowManager.AddContextChild(
                            //    parentWindow.m_WindowManager[parentWindow.m_TargetWindowContent],
                            //    value.GetComponent<RectTransform>(),
                            //    true);
                            parentWindow.m_WindowManager.SelectContextPlane(parentWindow.m_TargetWindowContent);
                            parentWindow.m_WindowManager.AddContextChild(value.GetComponent<RectTransform>(), true);
                        }
                        else if (parentEntry != null)
                        {
                            SetupParentRectTransform(value.GetComponent<RectTransform>(), parentEntry.ref_value.GetComponent<RectTransform>());
                        }
                        ForceRebuildLayoutImmediate();
                    }
                }
            }

            [Resources, SerializeField] private List<ItemEntry> childs = new();
            [Content, OnlyPlayMode, Ignore, SerializeField] private PropertiesWindow parentWindow;
            [Content, OnlyPlayMode, Ignore, SerializeField] private ItemEntry parentEntry;
            [Content] public readonly int layer;
            public readonly PropertiesWindow rootWindow;

            public ItemEntry(PropertiesWindow parent) : base(null)
            {
                childs = new();
                this.parentWindow = parent;
                this.rootWindow = parent;
                parent.m_Entrys.Add(this);
                layer = 0;
            }
            public ItemEntry(ItemEntry parent) : base(null)
            {
                childs = new();
                this.parentEntry = parent;
                this.rootWindow = parent.rootWindow;
                parent.childs.Add(this);
                layer = parent.layer + 1;
            }

            private void ForceRebuildLayoutImmediate()
            {
                if (ref_value != null)
                {
                    var rectTransform = ref_value.transform as RectTransform;
                    LayoutRebuilder.ForceRebuildLayoutImmediate(rectTransform);
                    RectTransformExtension.AdjustSizeToContainsChilds(rectTransform);
                }
                if (parentWindow)
                {
                    LayoutRebuilder.ForceRebuildLayoutImmediate(parentWindow.m_WindowManager[parentWindow.m_TargetWindowContent]);
                    RectTransformExtension.AdjustSizeToContainsChilds(parentWindow.m_WindowManager[parentWindow.m_TargetWindowContent]);
                }
                else
                {
                    var rgo = parentEntry.ref_value.transform as RectTransform;
                    LayoutRebuilder.ForceRebuildLayoutImmediate(rgo);
                    RectTransformExtension.AdjustSizeToContainsChilds(rgo);
                    parentEntry.ForceRebuildLayoutImmediate();
                }
            }

            public void DisableChilds(bool isForceRebuildLayoutImmediate)
            {
                foreach (var item in childs)
                {
                    item.ref_value.gameObject.SetActive(false);
                }
                if (isForceRebuildLayoutImmediate)
                    ForceRebuildLayoutImmediate();
            }

            public void Disable(bool isForceRebuildLayoutImmediate)
            {
                if (ref_value)
                {
                    ref_value.gameObject.SetActive(false);
                }
                DisableChilds(false);
                if (isForceRebuildLayoutImmediate)
                    ForceRebuildLayoutImmediate();
            }

            public void EnableChilds(bool isForceRebuildLayoutImmediate)
            {
                foreach (var item in childs)
                {
                    item.ref_value.gameObject.SetActive(true);
                }
                if (isForceRebuildLayoutImmediate)
                    ForceRebuildLayoutImmediate();
            }

            public void Enable(bool isForceRebuildLayoutImmediate)
            {
                EnableChilds(false);
                if (ref_value)
                {
                    ref_value.gameObject.SetActive(true);
                }
                if (isForceRebuildLayoutImmediate)
                    ForceRebuildLayoutImmediate();
            }

            public void Release()
            {
                if (ref_value)
                {
                    ref_value.gameObject.SetActive(false);
                    Destroy(ref_value.gameObject);
                }
                foreach (var item in childs)
                {
                    item.Release();
                }
                if (parentWindow != null)
                    parentWindow.m_Entrys.Remove(this);
                else
                    parentEntry.childs.Remove(this);
                ref_value = null;
            }

            ~ItemEntry()
            {
                Release();
            }
        }
        [Resources, SerializeField, HopeNotNull] private SO.Windows m_WindowsConfig;
        [Resources, SerializeField, HopeNotNull] private WindowManager m_WindowManager;
        [Setting, SerializeField, OnlyNotNullMode(nameof(m_WindowManager))] private int m_TargetWindowContent = 0;
        [Content, SerializeField, OnlyPlayMode] private List<ItemEntry> m_Entrys = new();
        [Resources, SerializeField, HopeNotNull] public PropertyListItem PropertyListItemPrefab;
        [Setting, Tooltip("RUNTIME MODE")] public PerformanceIndicator.PerformanceMode m_PerformanceMode = PerformanceIndicator.PerformanceMode.Quality;

        private void Start()
        {
            if (m_WindowsConfig == null)
                m_WindowsConfig = SO.Windows.GlobalInstance;
            if (m_WindowManager == null)
                m_WindowManager = GetComponent<WindowManager>();
        }

        private void Reset()
        {
            m_WindowsConfig = SO.Windows.GlobalInstance;
            m_WindowManager = GetComponent<WindowManager>();
            foreach (var entry in m_Entrys)
            {
                entry.Release();
            }
            m_Entrys = new();
        }

        private void FixedUpdate()
        {
            if ((m_PerformanceMode & PerformanceIndicator.PerformanceMode.L8) != 0)
                RectTransformExtension.AdjustSizeToContainsChilds(m_WindowManager[m_TargetWindowContent]);
        }

        public List<ItemEntry> CreateRootItemEntrysFromString(params string[] prefabs)
        {
            List<ItemEntry> result = new();
            foreach (string prefab in prefabs)
            {
                var current = ItemEntry.MakeItemWithInstantiate(prefab, this);
                result.Add(current);
                current.ref_value.gameObject.SetActive(true);
            }
            RectTransformExtension.AdjustSizeToContainsChilds(m_WindowManager[m_TargetWindowContent]);
            return result;
        }

        public List<ItemEntry> CreateRootPropertyItemEntrys(int count)
        {
            List<ItemEntry> result = new();
            while (count-- > 0)
            {
                var current = ItemEntry.MakeItemWithInstantiate(PropertyListItemPrefab, this);
                result.Add(current);
                current.ref_value.GetComponent<PropertyListItem>().Entry = current;
                current.ref_value.gameObject.SetActive(true);
            }
            RectTransformExtension.AdjustSizeToContainsChilds(m_WindowManager[m_TargetWindowContent]);
            return result;
        }
    }

    public class PropertyListItem : WindowUIModule, ITitle, IText
    {
        [Resources, SerializeField, OnlyNotNullMode] private UnityEngine.UI.Button m_rawButton;
        [Resources, SerializeField, OnlyNotNullMode(nameof(m_rawButton))] private float layerTab = 7.5f;
        [Resources, SerializeField, OnlyNotNullMode(nameof(m_rawButton))] private float layerHeight = 15f;
        [Resources, SerializeField, OnlyNotNullMode] private RectTransform dropdownImage;
        [Resources, SerializeField, OnlyNotNullMode] private Text m_buttonText;
        [Resources, SerializeField, OnlyNotNullMode, Header("Self Layer")] private RectTransform m_Layer;

        [Content, SerializeField] private PropertiesWindow.ItemEntry m_entry;
        [Content, Ignore, SerializeField] private bool m_folderStats = true;

        public PropertiesWindow.ItemEntry Entry
        {
            get => m_entry;
            set
            {
                if (this.gameObject.activeInHierarchy && m_entry != null)
                {
                    throw new InvalidOperationException();
                }
                m_entry = value;
                m_entry.ref_value = this;
            }
        }

        private void Relayer()
        {
            m_Layer.sizeDelta = new(m_entry != null ? layerTab * m_entry.layer : 0, layerHeight);
        }

        protected virtual void Start()
        {
            m_rawButton.gameObject.AddComponent<RectTransformExtension.AdjustSizeIgnore>();
            dropdownImage.gameObject.AddComponent<RectTransformExtension.AdjustSizeIgnore>();
            m_buttonText.gameObject.AddComponent<RectTransformExtension.AdjustSizeIgnore>();
            m_rawButton.onClick.AddListener(Switch);
            dropdownImage.eulerAngles = new(0, 0, 90);
        }

        protected virtual void OnEnable()
        {
            Relayer();
        }

        public bool IsFold
        {
            get => m_folderStats;
            set
            {
                if (value != m_folderStats)
                {
                    m_folderStats = value;
                    if (value)
                    {
                        FoldChilds();
                    }
                    else
                    {
                        UnfoldChilds();
                    }
                }
            }
        }

        public string title { get => m_buttonText.title; set => m_buttonText.title = value; }
        public string text { get => m_buttonText.text; set => m_buttonText.text = value; }

        public void Switch()
        {
            IsFold = !IsFold;
        }

        private void FoldChilds()
        {
            dropdownImage.eulerAngles = new(0, 0, 90);
            m_entry.DisableChilds(true);
        }
        private void UnfoldChilds()
        {
            m_entry.EnableChilds(true);
            dropdownImage.eulerAngles = new(0, 0, 0);
        }

        public List<ItemEntry> CreateSubPropertyItem([In] PropertiesWindow propertyWindow, int count)
        {
            List<ItemEntry> result = new();
            while (count-- > 0)
            {
                var item = ItemEntry.MakeItemWithInstantiate(propertyWindow.PropertyListItemPrefab, this.Entry);
                (item.ref_value as PropertyListItem).Entry = item;
                result.Add(item);
            }
            return result;
        }
        public List<ItemEntry> CreateSubPropertyItem(int count)
        {
            return CreateSubPropertyItem(Entry.rootWindow, count);
        }
    }
}

