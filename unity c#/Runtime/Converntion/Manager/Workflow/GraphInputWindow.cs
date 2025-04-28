using System;
using System.Collections;
using System.Collections.Generic;
using Convention.WindowsUI;
using Convention.WindowsUI.Variant;
using UnityEngine;
using UnityEngine.Events;

namespace Convention.Workflow
{
    public class GraphInputWindow : MonoSingleton<GraphInputWindow>, ITitle, IText
    {
        [Resources, SerializeField, OnlyNotNullMode] private Text Title;
        [Resources, SerializeField, OnlyNotNullMode] private Text Description;
        [Resources, SerializeField, OnlyNotNullMode] private ModernUIButton AddContentInput;

        [Resources, SerializeField, HopeNotNull, Header(nameof(HierarchyWindow))] private HierarchyWindow MyHierarchyWindow;

        private RegisterWrapper<GraphInputWindow> m_RegisterWrapper;

        private PropertiesWindow.ItemEntry StartNodeInputsTab, FunctionsTab, EndNodeOutputsTab;

        public string title { get => ((ITitle)this.Title).title; set => ((ITitle)this.Title).title = value; }
        public string text { get => ((IText)this.Description).text; set => ((IText)this.Description).text = value; }

        private class TitleClass : AnyClass, IHierarchyItemTitle
        {
            [InspectorDraw(InspectorDrawType.Text, false, false)]
            public string title;

            public TitleClass(string title)
            {
                this.title = title;
            }

            public string HierarchyItemTitle => title;
        }

        private void Start()
        {
            AddContentInput.AddListener(() =>
            {
                SharedModule.instance.OpenCustomMenu(this.transform as RectTransform, new SharedModule.CallbackData("test", go => { }));
            });
            m_RegisterWrapper = new(() =>
            {
                StartNodeInputsTab = MyHierarchyWindow.CreateRootItemEntryWithBinders(new TitleClass(nameof(StartNodeInputsTab)))[0];
                StartNodeInputsTab.GetHierarchyItem().title = WorkflowManager.Transformer("StartNodes");
                FunctionsTab = MyHierarchyWindow.CreateRootItemEntryWithBinders(new TitleClass(nameof(FunctionsTab)))[0];
                FunctionsTab.GetHierarchyItem().title = WorkflowManager.Transformer("Functions");
                EndNodeOutputsTab = MyHierarchyWindow.CreateRootItemEntryWithBinders(new TitleClass(nameof(EndNodeOutputsTab)))[0];
                EndNodeOutputsTab.GetHierarchyItem().title = WorkflowManager.Transformer("EndNodes");
            }, typeof(HierarchyWindow));
        }

        private void OnDestroy()
        {
            m_RegisterWrapper.Release();
        }
        private void Reset()
        {
            MyHierarchyWindow = GetComponent<HierarchyWindow>();    
        }

        public PropertiesWindow.ItemEntry RegisterOnHierarchyWindow(NodeInfo info)
        {
            PropertiesWindow.ItemEntry item = null;
            if (info is StartNodeInfo)
            {
                item = StartNodeInputsTab.GetHierarchyItem().CreateSubPropertyItemWithBinders(info)[0];
            }
            else if (info is EndNodeInfo)
            {
                item = EndNodeOutputsTab.GetHierarchyItem().CreateSubPropertyItemWithBinders(info)[0];
            }
            else if (info is StepNodeInfo sNode)
            {
                var parentItem = FunctionsTab.GetHierarchyItem();
                var menuEntry = parentItem.Entry.GetChilds().Find(x => (x.GetHierarchyItem().target as TitleClass).title == sNode.funcname);
                if (menuEntry == null)
                {
                    menuEntry = parentItem.CreateSubPropertyItemWithBinders(new TitleClass(sNode.funcname))[0];
                }
                item = menuEntry.GetHierarchyItem().CreateSubPropertyItemWithBinders(info)[0];
            }
            return item;
        }
    }
}
