using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using Convention.WindowsUI;
using Convention.WindowsUI.Variant;
using UnityEngine;

namespace Convention.Workflow
{
    [Serializable, ArgPackage]
    public class GraphNodeInfo : AnyClass
    {
        public string typename;

        [Ignore, NonSerialized] public GraphNode node;
        public string title = "";
        public Dictionary<string, GraphNodeSlotInfo> inmapping = new();
        public Dictionary<string, GraphNodeSlotInfo> outmapping = new();
        public Vector2 position = Vector2.zero;

        public void CopyFromNode()
        {
            title = node.title;
            inmapping.Clear();
            foreach (var (key, value) in node.m_Inmapping)
            {
                inmapping.Add(key, new()
                {
                    //parentNodeID = WorkflowManager.instance.GetGraphNodeID(value.Info.parentNode),
                    slotName = value.Info.slotName,
                    targetNodeID = WorkflowManager.instance.GetGraphNodeID(value.Info.targetNode),
                    targetSlotName = value.Info.slotName,
                    typeIndicator = value.Info.typeIndicator,
                    IsInmappingSlot = true
                });
            }
            outmapping.Clear();
            foreach (var (key, value) in node.m_Outmapping)
            {
                outmapping.Add(key, new()
                {
                    //parentNodeID = WorkflowManager.instance.GetGraphNodeID(value.Info.parentNode),
                    slotName = value.Info.slotName,
                    targetNodeID = WorkflowManager.instance.GetGraphNodeID(value.Info.targetNode),
                    targetSlotName = value.Info.slotName,
                    typeIndicator = value.Info.typeIndicator,
                    IsInmappingSlot = true
                });
            }
            position = node.transform.position;
        }

        [InspectorDraw(InspectorDrawType.Text, name: "节点名称")]
        public string GraphNodeTitle
        {
            get => node.title;
            set => node.title = value;
        }
        [InspectorDraw(InspectorDrawType.Text, false, false, name: "节点类型")]
        public string GraphNodeType => typename;

    }

    public class GraphNode : WindowsComponent, IOnlyFocusThisOnInspector, ITitle
    {
        [HideInInspector] public BehaviourContextManager Context;
        [Resources, OnlyNotNullMode, SerializeField] private Text Title;
        [Resources, OnlyNotNullMode, SerializeField] 
        private PropertiesWindow InSlotPropertiesWindow, OutSlotPropertiesWindow;
        private List<PropertiesWindow.ItemEntry> InSlots, OutSlots;

        public string title
        {
            get => ((ITitle)this.Title).title;
            set => ((ITitle)this.Title).title = value;
        }

        [HideInInspector] private GraphNodeInfo m_info;

        protected virtual void Start()
        {
            if (Context == null)
                Context = this.GetOrAddComponent<BehaviourContextManager>();
            Context.OnPointerDownEvent = BehaviourContextManager.InitializeContextSingleEvent(Context.OnPointerDownEvent, _ =>
            {
                if (this.m_info != null)
                    InspectorWindow.instance.SetTarget(this.m_info, null);
                else
                    Debug.LogError($"GraphNode<{this.GetType()}>={this}'s info is not setup", this);
            });
            Context.OnDragEvent = BehaviourContextManager.InitializeContextSingleEvent(Context.OnDragEvent, _ =>
            {
                foreach (var info in m_Inmapping)
                {
                    if (info.Value != null)
                        info.Value.SetDirty();
                }
            });
        }

        public GraphNodeInfo info
        {
            get => m_info;
            set
            {
                ClearLink();
                m_info = value;
                value.node = this;
                RefreshPosition();
                BuildLink();
            }
        }

        public void RefreshImmediate()
        {
            ClearLink();
            RefreshPosition();
            BuildLink();
        }
        public void RefreshPosition()
        {
            this.transform.position = info.position;
        }
        public void ClearLink()
        {
            m_Inmapping.Clear();
            m_Outmapping.Clear();
            foreach(var slot in InSlots)
            {
                slot.Release();
            }
            foreach (var slot in OutSlots)
            {
                slot.Release();
            }
        }
        public void BuildLink()
        {
            int InSlotCount = info.inmapping.Count;
            InSlots = InSlotPropertiesWindow.CreateRootItemEntries(InSlotCount);
            foreach(var (key,slotInfo) in info.inmapping)
            {
                InSlotCount--;
                var slot = InSlots[InSlotCount].ref_value.GetComponent<GraphNodeSlot>();
                m_Inmapping.Add(key, slot);
                slot.Info = new()
                {
                    parentNode = slotInfo.parentNode,
                    slotName = slotInfo.slotName,
                    typeIndicator = slotInfo.typeIndicator
                };
            }
        }

        public void LinkInslotToOtherNodeOutslot(
            [In, IsInstantiated(true)] GraphNode node,
            [In] string slotName,
            [In] string targetSlotName)
        {

        }
        public void LinkOutslotToOtherNodeInslot(
            [In, IsInstantiated(true)] GraphNode node,
            [In] string slotName,
            [In] string targetSlotName)
        {
            node.LinkInslotToOtherNodeOutslot(node, targetSlotName, slotName);
        }

        internal Dictionary<string, GraphNodeSlot> m_Inmapping = new();
        internal Dictionary<string, GraphNodeSlot> m_Outmapping = new();
    }
}
