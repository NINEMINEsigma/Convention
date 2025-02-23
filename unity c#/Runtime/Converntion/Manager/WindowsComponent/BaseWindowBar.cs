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

        [Resources, Setting, HopeNotNull] public SO.Windows WindowConfig;
        [Resources, SerializeField, HopeNotNull] private RectTransform BarPlane;
        [Resources, SerializeField, HopeNotNull] private WindowManager m_WindowManager;
        [Resources, HopeNotNull] public WindowUIModule ButtonPrefab;
        public void MinimizeWindow()
        {
            if (m_WindowManager)
            {
                m_WindowManager.WindowPlane.ExitMaximizeWindowMode();
            }
        }
        public void MaximizeWindow()
        {
            if (m_WindowManager)
            {
                m_WindowManager.WindowPlane.MaximizeWindow();
            }
        }
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

        [Serializable]
        public class RegisteredButtonWrapper : LeftValueReference<IButton>
        {
            public BaseWindowBar WindowBar;
            public RegisteredButtonWrapper([In]BaseWindowBar parentBar, [In] IButton ref_value) : base(ref_value)
            {
                WindowBar = parentBar;
            }
            public virtual void Disable()
            {
                (this.ref_value as WindowUIModule)?.gameObject.SetActive(false);
            }
            public virtual void Enable()
            {
                (this.ref_value as WindowUIModule)?.gameObject.SetActive(true);
            }
            public virtual void Release()
            {
                if(this.ref_value != null)
                {
                    Disable();
                    GameObject.Destroy((this.ref_value as WindowUIModule)?.gameObject);
                    this.ref_value = null;
                }
            }
            ~RegisteredButtonWrapper()
            {
                Release();
            }
        }
        [return :ReturnNotNull]
        public virtual RegisteredButtonWrapper RegisterButton()
        {
            return new RegisteredButtonWrapper(this, Instantiate(ButtonPrefab, BarPlane.transform) as IButton);
        }
    }
}
