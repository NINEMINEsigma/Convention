using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using UnityEngine;

namespace Convention.WindowsUI.Variant
{
    public class InspectorWindow : WindowsComponent, ISingleton<InspectorWindow>
    {
        public static InspectorWindow instance { get; private set; }
        private RegisterWrapper<InspectorWindow> registerWrapper;
        private object target;

        [Resources, SerializeField, OnlyNotNullMode] private ModernUIInputField m_ParentHashCodeField;
        private int m_lastParentHashCode = 0;
        [Resources, SerializeField, OnlyNotNullMode] private ModernUIInputField m_ThisHashCodeField;
        [Resources, SerializeField, HopeNotNull] private WindowManager m_WindowManager;
        [Resources, SerializeField, HopeNotNull] private PropertiesWindow m_PropertiesWindow;
        [Content, SerializeField] private List<PropertiesWindow.ItemEntry> m_currentEntries = new();

        private void Reset()
        {
            if (m_WindowManager == null)
                m_WindowManager = GetComponent<WindowManager>();
            if (m_PropertiesWindow == null)
                m_PropertiesWindow = GetComponent<PropertiesWindow>();
        }


        private void Start()
        {
            if (m_WindowManager == null)
                m_WindowManager = GetComponent<WindowManager>();
            if (m_PropertiesWindow == null)
                m_PropertiesWindow = GetComponent<PropertiesWindow>();

            registerWrapper = new(() => { });
            instance = this;

            m_ParentHashCodeField.gameObject.SetActive(false);
            m_ThisHashCodeField.gameObject.SetActive(false);
            m_ParentHashCodeField.AddListener(x =>
            {
                if (int.TryParse(x, out var code) && HierarchyWindow.instance.ContainsReference(code))
                {
                    m_lastParentHashCode = code;
                    HierarchyWindow.instance.SetHierarchyItemParent(
                        HierarchyWindow.instance.GetReferenceItem(target),
                        HierarchyWindow.instance.GetReferenceItem(HierarchyWindow.instance.GetReference(code))
                        );
                }
                else
                {
                    m_ParentHashCodeField.text = m_lastParentHashCode.ToString();
                }
            });
            m_ThisHashCodeField.interactable = false;
        }

        /// <summary>
        /// 设置引用以及对应的tab
        /// </summary>
        /// <param name="target"></param>
        /// <param name="item"></param>
        public void SetTarget([In] object target, [In, Opt] HierarchyItem item)
        {
            if (this.target == target)
                return;
            this.target = target;
            if (item)
            {
                m_ThisHashCodeField.text = target.GetHashCode().ToString();
                m_lastParentHashCode = item.Entry.GetParent() == null ? 0 : item.Entry.GetParent().ref_value.GetComponent<HierarchyItem>().target.GetHashCode();
            }
            m_ParentHashCodeField.gameObject.SetActive(item != null);
            m_ThisHashCodeField.gameObject.SetActive(item != null);
            RefreshImmediate();
        }
        public object GetTarget()
        {
            return this.target;
        }
        public void RefreshImmediate()
        {
            //if (FocusWindowIndictaor.instance.Target == this.rectTransform)
            {
                ClearWindow();
                BuildWindow();
            }
        }

        private void ClearWindow()
        {
            foreach (var entry in m_currentEntries)
            {
                entry.Release();
            }
            m_currentEntries.Clear();
        }
        private void BuildWindow()
        {
            var members =
                (from member in target.GetType().GetMembers(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.Static)
                 where member.GetCustomAttributes(typeof(InspectorDrawAttribute), true).Length != 0
                 select member).ToList();
            int offset = m_currentEntries.Count;
            if (target is Component component)
            {
                var transformItem = m_PropertiesWindow.CreateRootItemEntries(1)[0];
                transformItem.ref_value.GetComponent<PropertyListItem>().title = "Transform";
                transformItem.ref_value.GetComponent<InspectorItem>().SetTarget(target, new ValueWrapper(
                    () => component.transform,
                    (x) => throw new InvalidOperationException("Transform cannt be set"),
                    typeof(Transform)
                    ));
                m_currentEntries.Add(transformItem);
                offset++;
            }
            else if(target is GameObject go)
            {
                var transformItem = m_PropertiesWindow.CreateRootItemEntries(1)[0];
                transformItem.ref_value.GetComponent<PropertyListItem>().title = "Transform";
                transformItem.ref_value.GetComponent<InspectorItem>().SetTarget(target, new ValueWrapper(
                    () => go.transform,
                    (x) => throw new InvalidOperationException("Transform cannt be set"),
                    typeof(Transform)
                    ));
                m_currentEntries.Add(transformItem);
                offset++;
                int componentsCount = go.GetComponentCount();
                m_currentEntries.AddRange(m_PropertiesWindow.CreateRootItemEntries(componentsCount));
                for (int i = 0, e = componentsCount; i < e; i++)
                {
                    var x_component = go.GetComponentAtIndex(i);
                    m_currentEntries[i + offset].ref_value.GetComponent<PropertyListItem>().title = x_component.GetType().Name;
                    m_currentEntries[i + offset].ref_value.GetComponent<InspectorItem>().SetTarget(
                        target, () => InspectorWindow.instance.SetTarget(x_component, null));
                }
                offset += componentsCount;
            }
            m_currentEntries.AddRange(m_PropertiesWindow.CreateRootItemEntries(members.Count));
            for (int i = 0, e = members.Count; i < e; i++)
            {
                m_currentEntries[i + offset].ref_value.GetComponent<PropertyListItem>().title = members[i].Name;
                m_currentEntries[i+ offset].ref_value.GetComponent<InspectorItem>().SetTarget(target, members[i]);
            }
        }
    }
}
