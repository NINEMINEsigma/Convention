using System;
using System.Collections;
using System.Collections.Generic;
using System.Numerics;
using UnityEngine;
using UnityEngine.Events;

namespace Convention.WindowsUI.Variant
{
    public class PropertiesWindow : MonoAnyBehaviour
    {
        [Serializable, ArgPackage]
        public class ItemEntry : LeftValueReference<WindowUIModule>
        {
            #region Tools

            private static void SetupParentRectTransform([In] RectTransform child, [In] WindowManager parent)
            {
                parent.AddContextChild(child, true);
            }
            private static void SetupParentRectTransform([In] RectTransform child, [In] RectTransform parent)
            {
                if (parent.GetComponent<WindowManager>() != null)
                    SetupParentRectTransform(child, parent.GetComponent<WindowManager>());
                if (parent.GetComponents<BaseWindowPlane>().Length != 0)
                    parent.GetComponents<BaseWindowPlane>()[0].AddChild(child, true);
                else
                    RectTransformInfo.SetParentAndResize(parent, child, true);
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
                            SetupParentRectTransform(value.GetComponent<RectTransform>(), parentWindow.m_WindowManager);
                        }
                        else if (parentEntry != null)
                        {
                            SetupParentRectTransform(value.GetComponent<RectTransform>(), parentEntry.ref_value.GetComponent<RectTransform>());
                        }
                    }
                }
            }

            [Resources] private List<ItemEntry> childs = new();
            [Content, OnlyPlayMode, Ignore, SerializeField, WhenAttribute.Is(nameof(parentEntry), null)] private PropertiesWindow parentWindow;
            [Content, OnlyPlayMode, Ignore, SerializeField, WhenAttribute.Is(nameof(parentWindow), null)] private ItemEntry parentEntry;

            public ItemEntry(PropertiesWindow parent) : base(null)
            {
                this.parentWindow = parent;
                parent.m_Entrys.Add(this);
            }
            public ItemEntry(ItemEntry parent) : base(null)
            {
                this.parentEntry = parent;
                parent.childs.Add(this);
            }

            public void Disable()
            {
                if (ref_value)
                {
                    ref_value.gameObject.SetActive(false);
                }
                foreach (var item in childs)
                {
                    item.Disable();
                }
            }

            public void Enable()
            {
                foreach (var item in childs)
                {
                    item.Enable();
                }
                if (ref_value)
                {
                    ref_value.gameObject.SetActive(true);
                }
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
        [Content, SerializeField, OnlyPlayMode] private List<ItemEntry> m_Entrys = new();

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

        public List<ItemEntry> CreateRootItemEntrysFromString(params string[] prefabs)
        {
            List<ItemEntry> result = new();
            foreach (string prefab in prefabs)
            {
                result.Add(ItemEntry.MakeItemWithInstantiate(prefab, this));
            }
            return result;
        }
    }
}

