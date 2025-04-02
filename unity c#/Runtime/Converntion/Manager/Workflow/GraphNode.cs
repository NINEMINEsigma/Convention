using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using Convention.WindowsUI;
using Convention.WindowsUI.Variant;
using UnityEngine;

namespace Convention.Workflow
{
    [Serializable, ArgPackage]
    public class GraphNodeInfo : AnyClass
    {
        [Setting, Ignore, NonSerialized] public GraphNode node = null;
        [Setting] public int nodeID = -1;

        [InspectorDraw(InspectorDrawType.Text, name: "GraphNodeTitle")]
        public string GraphNodeTitle
        {
            get => WorkflowManager.instance.GetGraphNode(nodeID).title;
            set => WorkflowManager.instance.GetGraphNode(nodeID).title = value;
        }
        [InspectorDraw(InspectorDrawType.Text, false, false, name: "GraphNodeType")]
        [Setting] public string typename;
        [Content] public string title = "";
        [Setting] public Dictionary<string, GraphNodeSlotInfo> inmapping = new();
        [Setting] public Dictionary<string, GraphNodeSlotInfo> outmapping = new();
        [Content] public Vector2 position = Vector2.zero;

        public virtual GraphNodeInfo TemplateClone()
        {
            GraphNodeInfo result = new()
            {
                nodeID = -1,
                typename = typename,
                title = title,
                position = Vector2.zero
            };
            foreach (var (key, value) in inmapping)
            {
                result.inmapping.Add(key, value.TemplateClone());
            }
            foreach (var (key,value) in outmapping)
            {
                result.outmapping.Add(key, value.TemplateClone());
            }
            return result;
        }

        public virtual void CopyFromNode([In] GraphNode node)
        {
            nodeID = WorkflowManager.instance.GetGraphNodeID(node);
            title = node.title;
            position = node.transform.position;
        }
    }
    public class StartEvent : AnyClass
    {

    }

    [Serializable, ArgPackage]
    public class StartNodeInfo: GraphNodeInfo
    {
        [InspectorDraw(InspectorDrawType.Text, name: "Content")]
        [Setting] public string content;

        [Setting, OnlyPlayMode]
        public static ToolFile LoadFromLocal(params string[] fileTypes)
        {
            string content = null;
            PluginExtenion.SelectFileOnSystem(x => content = x, "*", "*", fileTypes);
            var file = new ToolFile(content);
            if (file.IsExist == false)
                throw new FileNotFoundException($"{content} not found");
            return file;
        }
        [Setting,OnlyPlayMode]
        public static ToolURL LoadFromURL(string content)
        {
            return new(content);
        }

    }

    public class GraphNode : WindowsComponent, IOnlyFocusThisOnInspector, ITitle
    {
        [HideInInspector] public BehaviourContextManager Context;
        [Resources, OnlyNotNullMode, SerializeField] private Text Title;
        [Resources, OnlyNotNullMode, SerializeField]
        private PropertiesWindow InSlotPropertiesWindow, OutSlotPropertiesWindow;

        private List<PropertiesWindow.ItemEntry> InSlots, OutSlots;

        private Dictionary<string, GraphNodeSlot> m_Inmapping = new();
        private Dictionary<string, GraphNodeSlot> m_Outmapping = new();

        [Resources, SerializeField, OnlyNotNullMode] private BaseWindowPlane InoutContainerPlane;

        public string title
        {
            get => ((ITitle)this.Title).title;
            set => ((ITitle)this.Title).title = value;
        }

        [HideInInspector] public GraphNodeInfo info { get; private set; }

        protected virtual void Start()
        {
            if (Context == null)
                Context = this.GetOrAddComponent<BehaviourContextManager>();
            Context.OnPointerDownEvent = BehaviourContextManager.InitializeContextSingleEvent(Context.OnPointerDownEvent, _ =>
            {
                if (this.info != null)
                    InspectorWindow.instance.SetTarget(this.info, null);
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

        public void SetupFromInfo([In] GraphNodeInfo value)
        {
            ClearLink();
            info = value;
            value.nodeID = WorkflowManager.instance.GetGraphNodeID(this);
            value.node = this;
            RefreshPosition();
            BuildLink();
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
            foreach (var slot in InSlots)
            {
                slot.Release();
            }
            foreach (var slot in OutSlots)
            {
                slot.Release();
            }
            InoutContainerPlane.AdjustSizeToContainsChilds();
        }
        private List<PropertiesWindow.ItemEntry> CreateGraphNodeSlots(int count)
        {
            return InSlotPropertiesWindow.CreateRootItemEntries(count);
        }
        public void BuildLink()
        {
            int InSlotCount = info.inmapping.Count;
            InSlots = CreateGraphNodeSlots(InSlotCount);
            foreach (var (key, slotInfo) in info.inmapping)
            {
                InSlotCount--;
                var slot = InSlots[InSlotCount].ref_value.GetComponent<GraphNodeSlot>();
                m_Inmapping.Add(key, slot);
                slot.SetupFromInfo(new()
                {
                    parentNode = slotInfo.parentNode,
                    slot = slot,
                    slotName = slotInfo.slotName,
                    typeIndicator = slotInfo.typeIndicator
                });
            }
            int OutSlotCount = info.outmapping.Count;
            OutSlots = CreateGraphNodeSlots(OutSlotCount);
            foreach (var (key, slotInfo) in info.outmapping)
            {
                OutSlotCount--;
                var slot = OutSlots[OutSlotCount].ref_value.GetComponent<GraphNodeSlot>();
                m_Outmapping.Add(key, slot);
                slot.SetupFromInfo(new()
                {
                    parentNode = slotInfo.parentNode,
                    slot = slot,
                    slotName = slotInfo.slotName,
                    typeIndicator = slotInfo.typeIndicator
                });
            }
            InoutContainerPlane.AdjustSizeToContainsChilds();
        }

        public void LinkInslotToOtherNodeOutslot(
            [In, IsInstantiated(true)] GraphNode other,
            [In] string slotName,
            [In] string targetSlotName)
        {
            GraphNodeSlot.Link(this.m_Inmapping[slotName], other.m_Outmapping[targetSlotName]);
        }
        public void LinkOutslotToOtherNodeInslot(
            [In, IsInstantiated(true)] GraphNode other,
            [In] string slotName,
            [In] string targetSlotName)
        {
            GraphNodeSlot.Link(this.m_Outmapping[slotName], other.m_Inmapping[targetSlotName]);
        }
        public void UnlinkInslot([In] string slotName)
        {
            GraphNodeSlot.Unlink(this.m_Inmapping[slotName]);
        }
        public void UnlinkOutslot( [In] string slotName)
        {
            GraphNodeSlot.Unlink(this.m_Outmapping[slotName]);
        }
    }

}
