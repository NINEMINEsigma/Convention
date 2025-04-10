using System;
using System.Collections.Generic;
using System.IO;
using Convention.WindowsUI;
using Convention.WindowsUI.Variant;
using UnityEngine;
using UnityEngine.EventSystems;

namespace Convention.Workflow
{
    [Serializable, ArgPackage]
    public class NodeInfo : AnyClass
    {
        /// <summary>
        /// 节点
        /// </summary>
        [Setting, Ignore, NonSerialized] public Node node = null;
        /// <summary>
        /// 节点ID
        /// </summary>
        [Setting] public int nodeID = -1;
        /// <summary>
        /// 节点类型
        /// </summary>
        [InspectorDraw(InspectorDrawType.Text, false, false, name: "GraphNodeType")]
        [Setting] public string typename;
        /// <summary>
        /// 节点标题
        /// </summary>
        [Content] public string title = "";
        /// <summary>
        /// 输入映射
        /// </summary>
        [Setting] public Dictionary<string, NodeSlotInfo> inmapping = new();
        /// <summary>
        /// 输出映射
        /// </summary>
        [Setting] public Dictionary<string, NodeSlotInfo> outmapping = new();
        /// <summary>
        /// 节点位置
        /// </summary>
        [Content] public Vector2 position = Vector2.zero;

        [InspectorDraw(InspectorDrawType.Text, name: "GraphNodeTitle")]
        public string GraphNodeTitle
        {
            get => node.title;
            set => this.title = node.title = value;
        }

        public virtual NodeInfo TemplateClone()
        {
            NodeInfo result = new()
            {
                nodeID = nodeID,
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

        public virtual void CopyFromNode([In] Node node)
        {
            nodeID = WorkflowManager.instance.GetGraphNodeID(node);
            title = node.title;
            position = node.transform.position;
            foreach (var (key, inslot) in node.m_Inmapping)
            {
                inmapping[key] = inslot.info;
            }
            foreach (var (key, outslot) in node.m_Outmapping)
            {
                outmapping[key] = outslot.info;
            }
        }

        public virtual Node Instantiate()
        {
            //throw new NotImplementedException("Node.Instantiate not implemented");
            string key = this.GetType().Name;
            if (key.EndsWith("Info"))
                key = key[..-4];
            return (WorkflowManager.instance.GraphNodePrefabs.Datas[key].uobjectValue as GameObject).GetComponent<Node>();
        }
    }

    public class Node : WindowsComponent, IOnlyFocusThisOnInspector, ITitle
    {
        [HideInInspector] public BehaviourContextManager Context;
        [Resources, OnlyNotNullMode, SerializeField] private Text Title;
        [Resources, OnlyNotNullMode, SerializeField]
        private PropertiesWindow InSlotPropertiesWindow, OutSlotPropertiesWindow;

        private List<PropertiesWindow.ItemEntry> InSlots, OutSlots;

        internal Dictionary<string, NodeSlot> m_Inmapping = new();
        internal Dictionary<string, NodeSlot> m_Outmapping = new();

        [Resources, SerializeField, OnlyNotNullMode] private BaseWindowPlane InoutContainerPlane;

        public string title
        {
            get => ((ITitle)this.Title).title;
            set => ((ITitle)this.Title).title = value;
        }

        [HideInInspector] public NodeInfo info { get; private set; }

        protected virtual void Start()
        {
            if (Context == null)
                Context = this.GetOrAddComponent<BehaviourContextManager>();
            Context.OnPointerDownEvent = BehaviourContextManager.InitializeContextSingleEvent(Context.OnPointerDownEvent, OnPointerDown);
            Context.OnDragEvent = BehaviourContextManager.InitializeContextSingleEvent(Context.OnDragEvent, OnDrag);
        }

        public void OnPointerDown(PointerEventData _)
        {
            if (this.info != null)
                InspectorWindow.instance.SetTarget(this.info, null);
            else
                Debug.LogError($"GraphNode<{this.GetType()}>={this}'s info is not setup", this);
        }

        public void OnDrag(PointerEventData _)
        {
            foreach (var info in m_Inmapping)
            {
                if (info.Value != null)
                    info.Value.SetDirty();
            }
        }

        public void SetupFromInfo([In] NodeInfo value)
        {
            ClearLink();
            info = value;
            int nodeID = WorkflowManager.instance.GetGraphNodeID(this);
            if (nodeID < 0)
            {
                this.info.node = this;
            }
            else
            {
                value.nodeID = nodeID;
                value.node = this;
                BuildLink();
            }
            RefreshPosition();
        }

        public void RefreshImmediate()
        {
            //TODO要在断连之前保存连接信息, 在重建后重新连接
            ClearLink();
            RefreshPosition();
            BuildLink();
        }
        public void RefreshPosition()
        {
            this.transform.position = info.position;
        }
        public virtual void ClearLink()
        {
            foreach (var (name, slot) in this.m_Inmapping)
            {
                NodeSlot.Unlink(slot);
                slot.SetDirty();
            }
            foreach (var (name,slot) in m_Outmapping)
            {
                NodeSlot.Unlink(slot);
                slot.SetDirty();
            }
        }
        public virtual void ClearSlots()
        {
            if(this.info==null)
            {
                return;
            }
            foreach (var slot in this.InSlots)
                slot.Release();
            foreach (var slot in this.OutSlots)
                slot.Release();
            InSlots.Clear();
            OutSlots.Clear();
            this.m_Inmapping.Clear();
            this.m_Outmapping.Clear();
        }
        protected List<PropertiesWindow.ItemEntry> CreateGraphNodeSlots(int count)
        {
            return InSlotPropertiesWindow.CreateRootItemEntries(count);
        }
        public virtual void BuildSlots()
        {
            int InSlotCount = info.inmapping.Count;
            InSlots = CreateGraphNodeSlots(InSlotCount);
            foreach (var (key, slotInfo) in info.inmapping)
            {
                InSlotCount--;
                var slot = InSlots[InSlotCount].ref_value.GetComponent<NodeSlot>();
                m_Inmapping.Add(key, slot);
                var info = slotInfo.TemplateClone();
                info.parentNode = this;
                slot.SetupFromInfo(info);
            }
            int OutSlotCount = info.outmapping.Count;
            OutSlots = CreateGraphNodeSlots(OutSlotCount);
            foreach (var (key, slotInfo) in info.outmapping)
            {
                OutSlotCount--;
                var slot = OutSlots[OutSlotCount].ref_value.GetComponent<NodeSlot>();
                m_Outmapping.Add(key, slot);
                var info = slotInfo.TemplateClone();
                info.parentNode = this;
                slot.SetupFromInfo(info);
            }
            InoutContainerPlane.AdjustSizeToContainsChilds();
        }
        public virtual void BuildLink()
        {
            foreach (var (slot_name,slot_info) in info.inmapping)
            {
                var targetNode = WorkflowManager.instance.GetGraphNode(slot_info.targetNodeID);
                if (targetNode != null)
                {
                    NodeSlot.Link(m_Inmapping[slot_name], targetNode.m_Outmapping[slot_info.targetSlotName]);
                }
                else
                {
                    NodeSlot.Unlink(m_Inmapping[slot_name]);
                }
            }
            foreach (var (slot_name, slot_info) in info.outmapping)
            {
                var targetNode = WorkflowManager.instance.GetGraphNode(slot_info.targetNodeID);
                if (targetNode != null)
                {
                    NodeSlot.Link(m_Outmapping[slot_name], targetNode.m_Inmapping[slot_info.targetSlotName]);
                }
                else
                {
                    NodeSlot.Unlink(m_Outmapping[slot_name]);
                }
            }
        }

        public void LinkInslotToOtherNodeOutslot(
            [In, IsInstantiated(true)] Node other,
            [In] string slotName,
            [In] string targetSlotName)
        {
            NodeSlot.Link(this.m_Inmapping[slotName], other.m_Outmapping[targetSlotName]);
        }
        public void LinkOutslotToOtherNodeInslot(
            [In, IsInstantiated(true)] Node other,
            [In] string slotName,
            [In] string targetSlotName)
        {
            NodeSlot.Link(this.m_Outmapping[slotName], other.m_Inmapping[targetSlotName]);
        }
        public void UnlinkInslot([In] string slotName)
        {
            NodeSlot.Unlink(this.m_Inmapping[slotName]);
        }
        public void UnlinkOutslot( [In] string slotName)
        {
            NodeSlot.Unlink(this.m_Outmapping[slotName]);
        }
    }

    [Serializable, ArgPackage]
    public class NodeResult : AnyClass
    {
        public int nodeID = -1;
        public string nodeTitle = "";
        public Dictionary<string, object> result = new();
    }

    [Serializable, ArgPackage]
    public class ContextResult : AnyClass
    {
        public string hashID = "";
        public List<NodeResult> results = new();
    }

    [Serializable, ArgPackage]
    public class DynamicNodeInfo:NodeInfo
    {

    }

    [Serializable, ArgPackage]
    public class StartNodeInfo : NodeInfo
    {

    }

    [Serializable, ArgPackage]
    public class StepNodeInfo : NodeInfo
    {
        public string funcname = "unknown";
    }

    [Serializable, ArgPackage]
    public class EndNodeInfo : NodeInfo
    {

    }
}
