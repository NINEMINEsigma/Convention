using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering;
using System;
using static Convention.WindowsUI.Variant.PropertiesWindow;

namespace Convention.WindowsUI.Variant
{
    [RequireComponent(typeof(PropertiesWindow))]
    public class HierarchyWindow : MonoSingleton<HierarchyWindow>
    {
        [Resources] public WindowManager windowManager;
        [Resources, SerializeField] private PropertiesWindow m_PropertiesWindow;
        private RegisterWrapper<HierarchyWindow> m_Register;

        private Dictionary<int, object> AllReferenceLinker = new();
        private Dictionary<object, int> AllReferenceLinker_R = new();
        private Dictionary<object, HierarchyItem> AllReferenceItemLinker = new();

        /// <summary>
        /// 添加引用以及对应的tab
        /// </summary>
        /// <param name="reference"></param>
        /// <param name="item"></param>
        public void AddReference([In] object reference, [In] HierarchyItem item)
        {
            int code = reference.GetHashCode();
            this.AllReferenceLinker[code] = reference;
            this.AllReferenceLinker_R[reference] = code;
            this.AllReferenceItemLinker[reference] = item;
        }
        public void RemoveReference([In] object reference)
        {
            int code = this.AllReferenceLinker_R[reference];
            this.AllReferenceLinker_R.Remove(reference);
            this.AllReferenceLinker.Remove(code);
            this.AllReferenceItemLinker.Remove(reference);
        }
        public object GetReference(int code) => this.AllReferenceLinker[code];
        public int GetReferenceCode([In] object reference) => this.AllReferenceLinker_R[reference];
        public HierarchyItem GetReferenceItem([In] object reference) => this.AllReferenceItemLinker[reference];
        public bool ContainsReference(int code) => this.AllReferenceLinker.ContainsKey(code);
        public bool ContainsReference(object reference) => this.AllReferenceLinker_R.ContainsKey(reference);

        public void SetHierarchyItemParent([In] HierarchyItem childItemTab, [In] HierarchyItem parentItemTab)
        {
            object target = childItemTab.target;
            childItemTab.Entry.Release();
            parentItemTab.CreateSubPropertyItem(target);
        }


        public List<ItemEntry> CreateRootItemEntries(params object[] binders)
        {
            List<ItemEntry> entries = m_PropertiesWindow.CreateRootItemEntries(binders.Length);
            for (int i = 0, e = binders.Length; i != e; i++)
            {
                var item = entries[i].ref_value.GetComponent<HierarchyItem>();
                item.target = binders[i];
                AddReference(binders[i], item);
            }
            return entries;
        }


        private void Start()
        {
            m_Register = new(() => { });
        }

        private void Reset()
        {
            windowManager = GetComponent<WindowManager>();
            m_PropertiesWindow = GetComponent<PropertiesWindow>();
            AllReferenceLinker = new();
        }
    }
}

