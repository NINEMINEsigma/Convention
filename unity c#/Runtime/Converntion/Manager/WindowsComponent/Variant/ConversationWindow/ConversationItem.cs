using System.Collections.Generic;
using System;
using static Convention.WindowsUI.Variant.PropertiesWindow;
using System.Reflection;
using UnityEngine;
using UnityEngine.UI;

namespace Convention.WindowsUI.Variant
{
    public class ConversationItem : PropertyListItem
    {
        [Resources, SerializeField] private Image m_Icon;
        [Resources, SerializeField] private Text m_Role;
        [Resources, SerializeField] private Text m_Text;
        [Setting] public float LineHeight = 25;

        public void Setup([In] string text, [In] string role, int lineSize)
        {
            m_Icon.sprite = ConversationWindow.instance.GetRoleIconSprite(role);
            m_Role.text = text;
            m_Text.text = text;
            var rect = this.transform as RectTransform;
            rect.sizeDelta = new(rect.sizeDelta.x, LineHeight * lineSize);
        }
    }
}
