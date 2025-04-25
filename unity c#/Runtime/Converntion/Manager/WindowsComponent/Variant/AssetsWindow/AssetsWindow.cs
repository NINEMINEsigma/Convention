using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.UI;

namespace Convention.WindowsUI.Variant
{
    [RequireComponent(typeof(PropertiesWindow))]
    public class AssetsWindow : MonoSingleton<AssetsWindow>
    {
        [Content, OnlyPlayMode, SerializeField, Header("Assets Stack")] private Stack<List<PropertiesWindow.ItemEntry>> m_EntriesStack = new();
        [Resources, OnlyNotNullMode, SerializeField] private PropertiesWindow m_PropertiesWindow;
        [Resources, OnlyNotNullMode, SerializeField, Tooltip("Back Button")] private Button m_BackButton;
        [Resources, OnlyNotNullMode, SerializeField, Tooltip("Path Text")] private Text m_PathTitle;
        [Content, OnlyPlayMode] public string CurrentTargetName;
        [Content, OnlyPlayMode, SerializeField] public List<string> pathContainer = new();
        private RegisterWrapper<AssetsWindow> m_RegisterWrapper;

        public PropertiesWindow MainPropertiesWindow => m_PropertiesWindow;

        public void UpdatePathText()
        {
            m_PathTitle.text = string.Join('/', pathContainer.ToArray()) +
                (string.IsNullOrEmpty(CurrentTargetName) ? "" : $":<color=blue>{CurrentTargetName}</color>");
        }

        protected virtual void Start()
        {
            m_BackButton.onClick.AddListener(() => Pop(true));
            UpdatePathText();
            m_RegisterWrapper = new(() => { });
        }

        protected virtual void Reset()
        {
            m_PropertiesWindow.m_PerformanceMode = PerformanceIndicator.PerformanceMode.L1;
            m_PropertiesWindow = GetComponent<PropertiesWindow>();
        }

        public void Push([In] string label, [In] List<PropertiesWindow.ItemEntry> entries, bool isRefreshTop)
        {
            var top = Peek();
            if (top != null)
                foreach (var entry in top)
                {
                    entry.Disable(false);
                }
            m_EntriesStack.Push(entries);
            foreach (var entry in entries)
            {
                entry.Enable(false);
            }
            if (isRefreshTop)
                RectTransformExtension.AdjustSizeToContainsChilds(m_PropertiesWindow.TargetWindowContent);
            pathContainer.Add(label);
            UpdatePathText();
        }
        [return: ReturnMayNull, When("m_EntriesStack is empty")]
        public List<PropertiesWindow.ItemEntry> Peek()
        {
            if (m_EntriesStack.Count == 0)
                return null;
            return m_EntriesStack.Peek();
        }
        [return: ReturnMayNull, When("m_EntriesStack is empty")]
        public List<PropertiesWindow.ItemEntry> Pop(bool isRefreshTop)
        {
            if (m_EntriesStack.Count <= 1)
                return null;
            var top = m_EntriesStack.Pop();
            if (top != null)
                foreach (var entry in top)
                {
                    entry.Disable(false);
                }
            var entries = Peek();
            if (entries != null)
                foreach (var entry in entries)
                {
                    entry.Enable(false);
                }
            if (isRefreshTop)
                RectTransformExtension.AdjustSizeToContainsChilds(m_PropertiesWindow.TargetWindowContent);
            if (pathContainer.Count != 0)
                pathContainer.RemoveAt(pathContainer.Count - 1);
            UpdatePathText();
            return top;
        }
    }
}
