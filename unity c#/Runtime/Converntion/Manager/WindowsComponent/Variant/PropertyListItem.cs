﻿using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using static Convention.WindowsUI.Variant.PropertiesWindow;

namespace Convention.WindowsUI.Variant
{
    public class PropertyListItem : WindowUIModule, ITitle, IText, IItemEntry
    {
        [Resources, SerializeField, OnlyNotNullMode] private Button m_rawButton;
        [Resources, SerializeField, OnlyNotNullMode(nameof(m_rawButton))] private float layerTab = 7.5f;
        [Resources, SerializeField, OnlyNotNullMode(nameof(m_rawButton))] private float layerHeight = 15f;
        [Resources, SerializeField, OnlyNotNullMode] private RectTransform dropdownImage;
        [Resources, SerializeField, OnlyNotNullMode] private Text m_buttonText;
        [Resources, SerializeField, OnlyNotNullMode, Header("Self Layer")] private RectTransform m_Layer;

        [Content, SerializeField] private ItemEntry m_entry;
        [Content, Ignore, SerializeField] private bool m_folderStats = true;

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

        private void Relayer()
        {
            m_Layer.sizeDelta = new(m_entry != null ? layerTab * m_entry.layer : 0, layerHeight);
        }

        protected virtual void Start()
        {
            m_rawButton.gameObject.AddComponent<RectTransformExtension.AdjustSizeIgnore>();
            dropdownImage.gameObject.AddComponent<RectTransformExtension.AdjustSizeIgnore>();
            m_buttonText.gameObject.AddComponent<RectTransformExtension.AdjustSizeIgnore>();
            m_rawButton.onClick.AddListener(Switch);
            dropdownImage.eulerAngles = new(0, 0, 90);
        }

        protected virtual void OnEnable()
        {
            Relayer();
        }

        public bool IsFold
        {
            get => m_folderStats;
            set
            {
                if (value != m_folderStats)
                {
                    m_folderStats = value;
                    if (value)
                    {
                        FoldChilds();
                    }
                    else
                    {
                        UnfoldChilds();
                    }
                }
            }
        }

        public string title { get => m_buttonText.title; set => m_buttonText.title = value; }
        public string text { get => m_buttonText.text; set => m_buttonText.text = value; }

        public void Switch()
        {
            IsFold = !IsFold;
        }

        protected virtual void FoldChilds()
        {
            dropdownImage.eulerAngles = new(0, 0, 90);
            m_entry.DisableChilds(true);
        }
        protected virtual void UnfoldChilds()
        {
            m_entry.EnableChilds(true);
            dropdownImage.eulerAngles = new(0, 0, 0);
        }

        public List<ItemEntry> CreateSubPropertyItem([In] PropertiesWindow propertyWindow, int count)
        {
            List<ItemEntry> result = new();
            while (count-- > 0)
            {
                var item = ItemEntry.MakeItemWithInstantiate(propertyWindow.ItemPrefab, this.Entry);
                (item.ref_value as PropertyListItem).Entry = item;
                result.Add(item);
            }
            return result;
        }
        public List<ItemEntry> CreateSubPropertyItem(int count)
        {
            return CreateSubPropertyItem(Entry.rootWindow, count);
        }

        [Content]
        public void AdjustSizeToContainsChilds()
        {
            RectTransformExtension.AdjustSizeToContainsChilds(transform as RectTransform);
        }
    }
}

