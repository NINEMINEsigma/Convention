using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Rendering;

namespace Convention.WindowsUI.Variant
{
    public class AssetsItem : WindowUIModule, IText, ITitle
    {
        // ------------------------

        private static Stack<List<PropertiesWindow.ItemEntry>> m_EntriesStack = new();

        // ------------------------

        [Setting, OnlyNotNullMode] public ScriptableObject Icons;


        [Resources, SerializeField] private Button m_RawButton;
        [Resources, SerializeField] private Text m_Text;
        [Content, SerializeField] private List<PropertiesWindow.ItemEntry> m_SubEntries = new();

        public string title { get => this.m_Text.title; set => this.m_Text.title = value; }
        public string text { get => this.m_Text.text; set => this.m_Text.text = value; }

        private void Start()
        {

        }

        
    }
}
