using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Convention.WindowsUI.Variant
{
    public class InspectorWindow : WindowsComponent,ISingleton<InspectorWindow>
    {
        public static InspectorWindow instance { get; private set; }
        private RegisterWrapper<InspectorWindow> registerWrapper;
        private IAnyClass target;

        [Resources,SerializeField,HopeNotNull]private WindowManager m_WindowManager;
        [Resources,SerializeField,HopeNotNull]private PropertiesWindow m_PropertiesWindow;
        [Content, SerializeField] private List<PropertiesWindow.ItemEntry> m_currentEntries = new();

        private void Reset()
        {
            if (m_WindowManager == null)
                m_WindowManager = GetComponent<WindowManager>();
            if (m_PropertiesWindow == null)
                m_PropertiesWindow = GetComponent<PropertiesWindow>();
        }

        private void Start()
        {
            if (m_WindowManager == null)
                m_WindowManager = GetComponent<WindowManager>();
            if (m_PropertiesWindow == null)
                m_PropertiesWindow = GetComponent<PropertiesWindow>();

            registerWrapper = new(() => { });
            instance = this;
        }

        public void SetTarget([In, Opt][ArgPackage] IAnyClass target)
        {
            this.target = target;
            RefreshImmediate();
        }
        public IAnyClass GetTarget()
        {
            return this.target;
        }
        public void RefreshImmediate()
        {
            if (FocusWindowIndictaor.instance.Target == this.rectTransform)
            {
                ClearWindow();
                BuildWindow();
            }
        }

        private void ClearWindow()
        {
            foreach (var entry in m_currentEntries)
            {
                entry.Release();
            }
            m_currentEntries.Clear();
        }
        private void BuildWindow()
        {

        }
    }
}
