using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace Convention.WindowsUI.Variant
{
    [RequireComponent(typeof(PropertiesWindow))]
    public class ConsoleWindow : MonoSingleton<ConsoleWindow>
    {
        [Resources, SerializeField, OnlyNotNullMode, Header("Bar Button Setting")] private WindowManager m_WindowManager;
        [Resources, SerializeField, OnlyNotNullMode(nameof(m_WindowManager))] private RectTransform m_root;
        [Resources, SerializeField, OnlyNotNullMode(nameof(m_WindowManager))] private RectTransform m_plane;
        [Content, SerializeField, OnlyPlayMode, Ignore, OnlyNotNullMode(nameof(m_WindowManager))] private int m_registeredIndex;
        public BaseWindowBar.RegisteredPageWrapper ConsoleWindowIndex { get; private set; }
        [Header("Property Window - ListView"), Resources, SerializeField, OnlyNotNullMode] private PropertiesWindow m_ListView;
        private List<PropertiesWindow.ItemEntry> m_entries = new();

        private void Start()
        {
            Application.logMessageReceived += (condition, stackTrace, type) =>
            {
                PropertiesWindow.ItemEntry entry = m_ListView.CreateRootItemEntries(1)[0];
                m_entries.Add(entry);
                entry.ref_value.GetComponent<>
            };
            ConsoleWindowIndex = m_WindowManager.CreateSubWindowWithBarButton(m_plane, m_root);
            ConsoleWindowIndex.button.AddListener(() =>
            {
                ConsoleWindowIndex.Select();
            });
        }


    }
}
