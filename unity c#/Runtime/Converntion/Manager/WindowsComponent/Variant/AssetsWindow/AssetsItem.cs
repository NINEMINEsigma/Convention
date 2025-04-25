using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using static Convention.WindowsUI.Variant.PropertiesWindow;

namespace Convention.WindowsUI.Variant
{
    public class AssetsItem : WindowUIModule, IText, ITitle, IItemEntry
    {
        public static int TextStringLimit = 16;
        public static AssetsItem FocusItem;

        public interface IAssetsItemInvoke
        {
            void OnAssetsItemInvoke(AssetsItem item);
            void OnAssetsItemFocus(AssetsItem item);
        }

        [Resources, SerializeField, OnlyNotNullMode] private Button m_RawButton;
        [Resources, SerializeField, OnlyNotNullMode] private Text m_Text;
        [Content, SerializeField] private string m_TextString;
        [Content, SerializeField] private List<ItemEntry> m_ChildEntries = new();
        [Content, SerializeField] private ItemEntry m_entry;
        [Setting] public bool HasChildLayer = true;
        public ItemEntry Entry
        {
            get => m_entry;
            set
            {
                if (this.gameObject.activeInHierarchy && m_entry != null)
                {
                    throw new InvalidOperationException();
                }
                m_entry = value;
                m_entry.ref_value = this;
            }
        }
        public Sprite ButtonSprite
        {
            get => m_RawButton.GetComponent<Image>().sprite;
            set => m_RawButton.GetComponent<Image>().sprite = value;
        }

        public List<ItemEntry> AddChilds(int count)
        {
            var entries = Entry.rootWindow.CreateRootItemEntries(false, count);
            m_ChildEntries.AddRange(entries);
            return entries;
        }
        public ItemEntry AddChild()
        {
            var entry = Entry.rootWindow.CreateRootItemEntries(false, 1)[0];
            m_ChildEntries.Add(entry);
            return entry;
        }
        public void RemoveChild([In] ItemEntry entry)
        {
            if (m_ChildEntries.Remove(entry))
                entry.Disable(true);
        }
        public void RemoveAllChilds()
        {
            foreach (var entry in m_ChildEntries)
            {
                entry.Disable(true);
            }
            m_ChildEntries.Clear();
        }
        public int ChildCount()
        {
            return m_ChildEntries.Count;
        }
        public ItemEntry GetChild(int index)
        {
            return m_ChildEntries[index];
        }

        public string title
        {
            get => m_TextString;
            set
            {
                m_TextString = value;
                if (value.Length > TextStringLimit)
                    this.m_Text.title = value[..(TextStringLimit - 3)] + "...";
                else
                    this.m_Text.title = value;
            }
        }
        public string text
        {
            get => title; set => title = value;
        }

        private void Start()
        {
            m_RawButton.onClick.AddListener(() =>
            {
                Invoke();
            });
        }

        public virtual void Invoke()
        {
            AssetsWindow.instance.CurrentTargetName = m_TextString;
            if (FocusItem != this)
            {
                FocusItem = this;
                if (FocusWindowIndictaor.instance != null)
                    FocusWindowIndictaor.instance.SetTargetRectTransform(this.transform as RectTransform);
                foreach (var component in this.GetComponents<IAssetsItemInvoke>())
                {
                    component.OnAssetsItemFocus(this);
                }
            }
            else
            {
                FocusItem = null;
                if (FocusWindowIndictaor.instance != null)
                    FocusWindowIndictaor.instance.SetTargetRectTransform(null);
                if (HasChildLayer)
                    Entry.rootWindow.GetComponent<AssetsWindow>().Push(title, m_ChildEntries, true);
                foreach (var component in this.GetComponents<IAssetsItemInvoke>())
                {
                    component.OnAssetsItemInvoke(this);
                }
            }
            AssetsWindow.instance.UpdatePathText();
        }
    }
}
