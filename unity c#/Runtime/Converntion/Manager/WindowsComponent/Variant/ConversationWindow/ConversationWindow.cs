using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

namespace Convention.WindowsUI.Variant
{
    public class ConversationWindow : MonoSingleton<ConversationWindow>
    {
        [Resources, OnlyNotNullMode] public WindowManager m_WindowManager;
        [Resources, SerializeField, OnlyNotNullMode] private PropertiesWindow m_PropertiesWindow;
        private RegisterWrapper<ConversationWindow> m_RegisterWrapper;

        [Resources, Header("HeadLine"), OnlyNotNullMode] public Image HeadIcon;
        [Resources, OnlyNotNullMode] public ModernUIInputField HeadText = new();
        [Resources] public List<Text> HeadTitles = new();
        private List<PropertiesWindow.ItemEntry> m_entries = new();

        [Resources, Header("Roles Icon"), SerializeField, HopeNotNull] private ScriptableObject m_RolesIcon;
        [Resources, SerializeField, OnlyNotNullMode] private Sprite m_DefaultRoleIcon;

        public Sprite GetRoleIconSprite([In] string role)
        {
            if (m_RolesIcon == null)
                return m_DefaultRoleIcon;
            if (m_RolesIcon.uobjects.TryGetValue(role, out var roleIcon_) && roleIcon_ is Sprite roleIcon)
                return roleIcon;
            return m_DefaultRoleIcon;
        }

        private void Reset()
        {
            m_WindowManager = GetComponent<WindowManager>();
            m_PropertiesWindow = GetComponent<PropertiesWindow>();
        }

        private void Start()
        {
            m_RegisterWrapper = new(() =>
            {

            });
        }
        private void OnDestroy()
        {
            m_RegisterWrapper.Release();
        }

        public void SetHeadText(string text)
        {
            HeadText.text = text;
        }

        public void SetHeadTitle(int index, string text)
        {
            HeadTitles[index].text = text;
        }

        [Resources, Header("InputField")] private ModernUIInputField m_InputField;
        public delegate void MessageListener([In] string message);
        public event MessageListener messageListener;

        public void SendMessage()
        {
            messageListener?.Invoke(m_InputField.text);
            m_InputField.text = "";
        }

        public void CreateNewMessageListItem()
        {
            PropertiesWindow.ItemEntry entry = m_PropertiesWindow.CreateRootItemEntries(1)[0];
            m_entries.Add(entry);
            var item = entry.ref_value.GetComponent<ConversationItem>();
        }
    }
}
