using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using static Convention.WindowsUI.Variant.PropertiesWindow;

namespace Convention.WindowsUI.Variant
{
    public class AssetsItem : WindowUIModule, IText, ITitle, IItemEntry
    {
        public static AssetsItem FocusItem;

        public interface IAssetsItemInvoke
        {
            void OnAssetsItemInvoke(AssetsItem item);
            void OnAssetsItemFocus(AssetsItem item);
        }

        [Resources, SerializeField, OnlyNotNullMode] private Button m_RawButton;
        [Resources, SerializeField, OnlyNotNullMode] private Text m_Text;
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

        public string title { get => this.m_Text.title; set => this.m_Text.title = value; }
        public string text { get => this.m_Text.text; set => this.m_Text.text = value; }

        private void Start()
        {
            m_RawButton.onClick.AddListener(() =>
            {
                Invoke();
            });
        }

        public virtual void Invoke()
        {
            if (FocusItem != this)
            {
                FocusItem = this;
                FocusWindowIndictaor.instance.SetTargetRectTransform(this.transform as RectTransform);
                this.BroadcastMessage(nameof(IAssetsItemInvoke.OnAssetsItemFocus), this, SendMessageOptions.DontRequireReceiver);
            }
            else
            {
                FocusItem = null;
                FocusWindowIndictaor.instance.SetTargetRectTransform(null);
                if (HasChildLayer)
                    (Entry.rootWindow as AssetsWindow).Push(title, m_ChildEntries, true);
                this.BroadcastMessage(nameof(IAssetsItemInvoke.OnAssetsItemInvoke), this, SendMessageOptions.DontRequireReceiver);
            }
        }
    }
}
