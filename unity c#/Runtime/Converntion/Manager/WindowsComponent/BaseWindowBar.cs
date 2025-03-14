using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System;

namespace Convention.WindowsUI
{
    public class BaseWindowBar : WindowsComponent
    {
        // -----------------

        private bool use_VerticalLayoutGroup => layoutGroupType == LayoutGroupType.VerticalLayoutGroup && verticalLayoutGroup;
        private bool use_HorizontalLayoutGroup => layoutGroupType == LayoutGroupType.HorizontalLayoutGroup && horizontalLayoutGroup;
        private bool use_GridLayoutGroup => layoutGroupType == LayoutGroupType.GridLayoutGroup && gridLayoutGroup;

        // -----------------
        public bool hasLayoutGroup => layoutGroupType != LayoutGroupType.None;

        [Resources, Setting, HopeNotNull] public SO.Windows WindowConfig;
        [Resources, SerializeField, HopeNotNull] private RectTransform BarPlane;
        [Resources, SerializeField, HopeNotNull] private WindowManager m_WindowManager;
        [Resources, HopeNotNull] public WindowUIModule ButtonPrefab;
        [Content, OnlyPlayMode]
        public void MinimizeWindow()
        {
            if (m_WindowManager)
            {
                m_WindowManager.WindowPlane.ExitMaximizeWindowMode();
            }
        }
        [Content, OnlyPlayMode]
        public void MaximizeWindow()
        {
            if (m_WindowManager)
            {
                m_WindowManager.WindowPlane.MaximizeWindow();
            }
        }
        [Content, OnlyPlayMode]
        public void CloseWindow()
        {
            if (m_WindowManager)
            {
                m_WindowManager.CloseWindow();
            }
        }

        public enum LayoutGroupType
        {
            VerticalLayoutGroup,
            HorizontalLayoutGroup,
            GridLayoutGroup,
            None
        }
        [Setting] public LayoutGroupType layoutGroupType = LayoutGroupType.VerticalLayoutGroup;
        [Resources, Setting, SerializeField, Header("Vertical Layout Group Setting")]
        [HopeNotNull, WhenAttribute.Is(nameof(layoutGroupType), LayoutGroupType.VerticalLayoutGroup)]
        private VerticalLayoutGroup verticalLayoutGroup;
        [Resources, Setting, SerializeField, Header("Horizontal Layout Group Setting")]
        [HopeNotNull, WhenAttribute.Is(nameof(layoutGroupType), LayoutGroupType.HorizontalLayoutGroup)]
        private HorizontalLayoutGroup horizontalLayoutGroup;
        [Resources, Setting, SerializeField, Header("Grid Layout Group")]
        [HopeNotNull, WhenAttribute.Is(nameof(layoutGroupType), LayoutGroupType.GridLayoutGroup)]
        private GridLayoutGroup gridLayoutGroup;

        public void Reset()
        {
            WindowConfig = Resources.Load<SO.Windows>(SO.Windows.GlobalWindowsConfig);
            BarPlane = rectTransform;
            ResetWindowManager();
            ButtonPrefab = WindowConfig.GetWindowsUI<IButton>(nameof(ModernUIButton)) as WindowUIModule;
            layoutGroupType = LayoutGroupType.HorizontalLayoutGroup;
            ResetLayoutGroups(false);
        }

        private void ResetLayoutGroups(bool isDestroy)
        {
            if (verticalLayoutGroup == null)
                verticalLayoutGroup = BarPlane.GetComponent<VerticalLayoutGroup>();
            if (horizontalLayoutGroup == null)
                horizontalLayoutGroup = BarPlane.GetComponent<HorizontalLayoutGroup>();
            if (gridLayoutGroup == null)
                gridLayoutGroup = BarPlane.GetComponent<GridLayoutGroup>();
            if (!isDestroy)
                return;
            if (verticalLayoutGroup && layoutGroupType != LayoutGroupType.VerticalLayoutGroup)
                Destroy(verticalLayoutGroup);
            if (horizontalLayoutGroup && layoutGroupType != LayoutGroupType.HorizontalLayoutGroup)
                Destroy(horizontalLayoutGroup);
            if (gridLayoutGroup && layoutGroupType != LayoutGroupType.GridLayoutGroup)
                Destroy(gridLayoutGroup);
        }
        private void ResetWindowManager()
        {
            m_WindowManager = null;
            for (Transform item = transform; m_WindowManager == null && item != null; item = item.parent)
            {
                m_WindowManager = item.gameObject.GetComponent<WindowManager>();
            }
        }

        private void Start()
        {
            if (BarPlane == null)
                BarPlane = rectTransform;
            if (m_WindowManager == null)
            {
                ResetWindowManager();
            }
            ResetLayoutGroups(true);
        }

        private IButton InstantiateButton()
        {
            return Instantiate(ButtonPrefab, BarPlane.transform).GetComponents<IButton>()[0];
        }

        [Serializable]
        public class RegisteredButtonWrapper
        {
            public readonly BaseWindowBar WindowBar;
            public IButton button;
            public WindowUIModule buttonModule;
            public RegisteredButtonWrapper([In] BaseWindowBar parentBar, [In] IButton button)
            {
                WindowBar = parentBar;
                this.button = button;
                buttonModule = button as WindowUIModule;
            }
            public virtual void Disable()
            {
                if (buttonModule)
                {
                    buttonModule.gameObject.SetActive(false);
                    if (WindowBar.useGUILayout)
                    {
                        LayoutRebuilder.ForceRebuildLayoutImmediate(WindowBar.BarPlane);
                    }
                }
            }
            public virtual void Enable()
            {
                if (buttonModule)
                {
                    buttonModule.gameObject.SetActive(true);
                    if (WindowBar.useGUILayout)
                    {
                        LayoutRebuilder.ForceRebuildLayoutImmediate(WindowBar.BarPlane);
                    }
                }
            }
            public virtual void Release()
            {
                if (button == null)
                {
                    throw new InvalidOperationException("wrapper was released");
                }
                if (buttonModule)
                {
                    Disable();
                    Destroy(buttonModule.gameObject);
                    if (WindowBar.useGUILayout)
                    {
                        LayoutRebuilder.ForceRebuildLayoutImmediate(WindowBar.BarPlane);
                    }
                }
                button = null;
            }
            ~RegisteredButtonWrapper()
            {
                Release();
            }
        }
        [return: ReturnNotNull, ReturnVirtual]
        public virtual RegisteredButtonWrapper RegisterButton()
        {
            return new RegisteredButtonWrapper(this, InstantiateButton());
        }
        [Serializable]
        public class RegisteredPageWrapper : RegisteredButtonWrapper
        {
            [SerializeField]private int PageIndex = -1;
            [SerializeField]private RectTransform plane, root;
            public RegisteredPageWrapper(RectTransform plane, RectTransform root, [In] BaseWindowBar parentBar, [In] IButton button) : base(parentBar, button)
            {
                button.AddListener(() => WindowBar.m_WindowManager.SelectContextPlane(PageIndex));
                PageIndex = parentBar.m_WindowManager.AddContextPlane(plane, root);
                this.plane = plane;
                this.root = root;
            }
            public RegisteredPageWrapper(RectTransform plane, [In] BaseWindowBar parentBar, [In] IButton button) : base(parentBar, button)
            {
                button.AddListener(() => WindowBar.m_WindowManager.SelectContextPlane(PageIndex));
                PageIndex = parentBar.m_WindowManager.AddContextPlane(plane);
                this.plane = plane;
            }
            public override void Disable()
            {
                if (PageIndex < 0)
                {
                    return;
                }
                WindowBar.m_WindowManager.RemoveContextPlane(PageIndex);
                PageIndex = -1;
                base.Disable();
            }
            public override void Enable()
            {
                if (PageIndex < 0)
                {
                    WindowBar.m_WindowManager.AddContextPlane(plane, root);
                    base.Enable();
                }
            }
            public override void Release()
            {
                if (Application.isPlaying)
                {
                    if (!plane)
                        throw new InvalidOperationException("page was released");
                    else
                        return;
                }
                if (root)
                {
                    root.gameObject.SetActive(false);
                    Destroy(root);
                }
                else
                {
                    plane.gameObject.SetActive(false);
                    Destroy(plane);
                }
                base.Release();
                plane = null;
                root = null;
            }
            public virtual void Select()
            {
                if (PageIndex < 0)
                    return;
                WindowBar.m_WindowManager.SelectContextPlane(PageIndex);
            }
        }
        [return: ReturnNotNull, ReturnVirtual]
        public virtual RegisteredPageWrapper RegisterPage([In] RectTransform plane, [In] RectTransform root)
        {
            return new RegisteredPageWrapper(plane, root, this, InstantiateButton());
        }
        [return: ReturnNotNull, ReturnVirtual]
        public virtual RegisteredPageWrapper RegisterPage([In] RectTransform plane)
        {
            return new RegisteredPageWrapper(plane, this, InstantiateButton());
        }

        public virtual IEnumerable<IButton> GetAllButton()
        {
            List<IButton> result = new();
            foreach(IButton button in BarPlane.transform)
            {
                result.Add(button);
            }
            return result;
        }
    }
}
