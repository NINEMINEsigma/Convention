using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;


namespace Convention.WindowsUI.Variant
{
    [RequireComponent(typeof(PropertiesWindow))]
    public class ConsoleWindow : MonoSingleton<ConsoleWindow>
    {
        [Resources, SerializeField, OnlyNotNullMode, Header("Bar Button Setting")] private WindowManager m_WindowManager;
        [Resources, SerializeField, OnlyNotNullMode(nameof(m_WindowManager))] private RectTransform m_root;
        [Resources, SerializeField, OnlyNotNullMode(nameof(m_WindowManager))] private RectTransform m_plane;
        public int ConsoleWindowIndex { get; private set; }
        [Header("Property Window - ListView"), Resources, SerializeField, OnlyNotNullMode] private PropertiesWindow m_ListView;
        private List<PropertiesWindow.ItemEntry> m_entries = new();
        [Resources, SerializeField, OnlyNotNullMode] private ModernUIInputField StackTrace;

        [Header("Message Switch"), Resources, SerializeField, OnlyNotNullMode] private ModernUIToggle m_MessageSwitch;
        [Resources, SerializeField, OnlyNotNullMode] private ModernUIToggle m_WarningSwitch;
        [Resources, SerializeField, OnlyNotNullMode] private ModernUIToggle m_VitalSwitch;
        [Resources, SerializeField, OnlyNotNullMode] private Button m_ClearLogs;

        [Setting] public string ConsoleButtonName = "Console";

        public void ClearLog()
        {
            foreach (var entry in m_entries)
            {
                entry.Release();
            }
            m_entries.Clear();
        }

        public void Log(string condition, string stackTrace, LogType type = LogType.Log)
        {
            ConsoleListItem item;
            string color;
            GenerateLogItem(type, out item, out color);
            item.SetupMessage(condition, stackTrace, color, type);
        }

        private void GenerateLogItem(LogType type, out ConsoleListItem item, out string color)
        {
            bool isActive = type switch
            {
                LogType.Log => m_MessageSwitch.ref_value,
                LogType.Warning => m_WarningSwitch.ref_value,
                _ => m_VitalSwitch.ref_value
            };
            PropertiesWindow.ItemEntry entry = m_ListView.CreateRootItemEntries(isActive, 1)[0];
            m_entries.Add(entry);
            item = entry.ref_value.GetComponent<ConsoleListItem>();
            color = type switch
            {
                LogType.Log => "white",
                LogType.Warning => "yellow",
                _ => "red"
            };
        }

        public void Log(string condition, string stackTrace, LogType type, string format)
        {
            ConsoleListItem item;
            string color;
            GenerateLogItem(type, out item, out color);
            item.SetupMessage(condition, stackTrace, color, type, format);
        }

        public void SetStackTrace(string str)
        {
            StackTrace.text = str;
        }

        private void Start()
        {
            Application.logMessageReceived -= Log;
            Application.logMessageReceived += Log;
            ConsoleWindowIndex = m_WindowManager.AddContextPlane(m_plane, m_root);
            var buttonWrapper = m_WindowManager.CreateWindowBarButton(() =>
            {
                m_WindowManager.SelectContextPlane(ConsoleWindowIndex);
            });
            (buttonWrapper.button as ITitle).title = ConsoleButtonName;
            StackTrace.interactable = false;

            m_MessageSwitch.ref_value = true;
            m_WarningSwitch.ref_value = true;
            m_VitalSwitch.ref_value = true;

            m_MessageSwitch.AddListener(x =>
            {
                foreach (var entry in m_entries)
                {
                    var item = entry.ref_value.GetComponent<ConsoleListItem>();
                    if (item.logType == LogType.Log)
                    {
                        item.gameObject.SetActive(x);
                    }
                }
            });
            m_WarningSwitch.AddListener(x =>
            {
                foreach (var entry in m_entries)
                {
                    var item = entry.ref_value.GetComponent<ConsoleListItem>();
                    if (item.logType == LogType.Warning)
                    {
                        item.gameObject.SetActive(x);
                    }
                }
            });
            m_VitalSwitch.AddListener(x =>
            {
                foreach (var entry in m_entries)
                {
                    var item = entry.ref_value.GetComponent<ConsoleListItem>();
                    if (item.logType != LogType.Log && item.logType != LogType.Warning)
                    {
                        item.gameObject.SetActive(x);
                    }
                }
            });
            m_ClearLogs.onClick.AddListener(() =>
            {
                foreach (var entry in m_entries)
                {
                    entry.Release();
                }
                m_entries.Clear();
            });

            m_WindowManager.SelectContextPlane(0);
        }

        [Setting, OnlyPlayMode]
        public void TestLog()
        {
            Debug.Log("Test");
        }
        [Setting, OnlyPlayMode]
        public void TestWarning()
        {
            Debug.LogWarning("Test");
        }
        [Setting, OnlyPlayMode]
        public void TestError()
        {
            Debug.LogError("Test");
        }
    }
}
