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

        [Resources, SerializeField, OnlyNotNullMode] private Text m_TypeText;

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
        /// <returns>是否与传入的target相同</returns>
        [return: When("当传入的target与被设置为target的实例相同")]
        public bool SetTarget([In] object target, [In, Opt] HierarchyItem item)
        {
            bool result = true;
            if (target is GameObject go)
            {
                var only = ConventionUtility.SeekComponent<IOnlyFocusThisOnInspector>(go);
                if (only != null)
                {
                    result = true;
                    target = only;
                }
            }
            if (this.target == target)
                return true;
            this.target = target;
            if (item)
            {
                m_ThisHashCodeField.text = target.GetHashCode().ToString();
                m_lastParentHashCode = item.Entry.GetParent() == null ? 0 : item.Entry.GetParent().ref_value.GetComponent<HierarchyItem>().target.GetHashCode();
            }
            m_ParentHashCodeField.gameObject.SetActive(item != null);
            m_ThisHashCodeField.gameObject.SetActive(item != null);
            RefreshImmediate();
            return result;
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
                if (target != null)
                    BuildWindow();
            }
        }

        public void ClearWindow()
        {
            m_TypeText.text = "Not Selected";
            foreach (var entry in m_currentEntries)
            {
                entry.Release();
            }
            m_currentEntries.Clear();
        }
        private void BuildWindow()
        {
            m_TypeText.text = target.GetType().FullName;
            var members =
                (from member in target.GetType().GetMembers(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.Static)
                 where member.GetCustomAttributes(typeof(InspectorDrawAttribute), true).Length != 0
                 select member).ToList();
            int offset = m_currentEntries.Count;
            // Component or GameObject
            if (target is Component component)
            {
                offset = GenerateComponentTransformModule(offset, component.transform);
            }
            else if (target is GameObject go)
            {
                offset = GenerateComponentTransformModule(offset, go.transform);
                offset = GenerateGameObjectComponentModules(offset, go);
            }
            // Main
            m_currentEntries.AddRange(m_PropertiesWindow.CreateRootItemEntries(members.Count));
            for (int i = 0, e = members.Count; i < e; i++)
            {
                m_currentEntries[i + offset].ref_value.GetComponent<PropertyListItem>().title = members[i].Name;
                m_currentEntries[i + offset].ref_value.GetComponent<InspectorItem>().SetTarget(target, members[i]);
            }
            offset += members.Count;
            // End To GameObject
            if (target is Component component_1)
            {
                offset = GenerateToGameObjectButtonModule(offset, component_1);
            }

            int GenerateGameObjectComponentModules(int offset, GameObject go)
            {
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
                {
                    var DestroyGameObjectButton = m_PropertiesWindow.CreateRootItemEntries(1)[0];
                    DestroyGameObjectButton.ref_value.GetComponent<PropertyListItem>().title = "Destroy GameObject";
                    DestroyGameObjectButton.ref_value.GetComponent<InspectorItem>().SetTarget(target, () =>
                    {
                        HierarchyWindow.instance.GetReferenceItem(go).Entry.Release();
                        HierarchyWindow.instance.RemoveReference(go);
                        GameObject.Destroy(go);
                        InspectorWindow.instance.ClearWindow();
                    });
                    m_currentEntries.Add(DestroyGameObjectButton);
                    offset++;
                }
                {
                    var DestroyGameObjectButton = m_PropertiesWindow.CreateRootItemEntries(1)[0];
                    DestroyGameObjectButton.ref_value.GetComponent<PropertyListItem>().title = "GameObject Active";
                    DestroyGameObjectButton.ref_value.GetComponent<InspectorItem>().SetTarget(target, new ValueWrapper(
                        () => go.activeSelf,
                        (x) => go.SetActive((bool)x),
                        typeof(bool)
                        ));
                    m_currentEntries.Add(DestroyGameObjectButton);
                    offset++;
                }
                return offset;
            }

            int GenerateComponentTransformModule(int offset, Transform transform)
            {
                var transformItem = m_PropertiesWindow.CreateRootItemEntries(1)[0];
                transformItem.ref_value.GetComponent<PropertyListItem>().title = "Transform";
                transformItem.ref_value.GetComponent<InspectorItem>().SetTarget(target, new ValueWrapper(
                    () => transform,
                    (x) => throw new InvalidOperationException("Transform cannt be set"),
                    typeof(Transform)
                    ));
                m_currentEntries.Add(transformItem);
                offset++;
                return offset;
            }

            int GenerateToGameObjectButtonModule(int offset, Component component)
            {
                var toGameObjectButton = m_PropertiesWindow.CreateRootItemEntries(1)[0];
                toGameObjectButton.ref_value.GetComponent<PropertyListItem>().title = "To GameObject";
                toGameObjectButton.ref_value.GetComponent<InspectorItem>().SetTarget(target, () => SetTarget(component.gameObject, null));
                m_currentEntries.Add(toGameObjectButton);
                offset++;
                return offset;
            }
        }
    }
}
