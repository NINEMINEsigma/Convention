using System;
using System.Collections;
using System.Collections.Generic;
using Convention.WindowsUI;
using Convention.WindowsUI.Variant;
using UnityEngine;

namespace Convention.Workflow
{
    [Serializable, ArgPackage]
    public class GraphNodeInfo : AnyClass
    {
        [Ignore, NonSerialized] public GraphNode node;
        public string title = "";
        public Dictionary<string, int> inmappingNode = new();
        public Dictionary<string, int> outmappingNode = new();
        public Dictionary<string, string> inmappingSlot = new();
        public Dictionary<string, string> outmappingSlot = new();
        public Dictionary<string, string> inmappingTypeIndicator = new();
        public Dictionary<string, string> outmappingTypeIndicator = new();
        public string type;
        public Vector2 position = Vector2.zero;

        public void SynchronizeWithNode()
        {
            title = node.title;
            inmappingNode.Clear();
            inmappingSlot.Clear();
            foreach (var (key, value) in node.m_InmappingNode)
            {
                inmappingNode.Add(key, WorkflowManager.instance.GetGraphNodeID(value));
                if (node.m_InmappingSlot.TryGetValue(key, out var slot))
                    inmappingSlot.Add(key, slot);
            }
            outmappingNode.Clear();
            outmappingSlot.Clear();
            foreach (var (key, value) in node.m_OutmappingNode)
            {
                outmappingNode.Add(key, WorkflowManager.instance.GetGraphNodeID(value));
                if (node.m_OutmappingSlot.TryGetValue(key, out var slot))
                    outmappingSlot.Add(key, slot);
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
        public string GraphNodeType => type;

    }

    public class GraphNode : WindowsComponent, IOnlyFocusThisOnInspector, ITitle
    {
        [HideInInspector] public BehaviourContextManager Context;
        [Resources, OnlyNotNullMode, SerializeField] private Text Title;

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

        }
        public void BuildLink()
        {

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

        internal Dictionary<string, GraphNode> m_InmappingNode = new();
        internal Dictionary<string, GraphNode> m_OutmappingNode = new();
        internal Dictionary<string, string> m_InmappingSlot = new();
        internal Dictionary<string, string> m_OutmappingSlot = new();
    }
}
