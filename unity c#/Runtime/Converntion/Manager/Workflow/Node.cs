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

        public NodeInfo()
        {
            title = WorkflowManager.Transformer(typename = this.GetType().Name[..^4]);
        }

        protected virtual NodeInfo CreateTemplateNodeInfoBySelfType()
        {
            return new NodeInfo();
        }
        public virtual NodeInfo TemplateClone()
        {
            NodeInfo result = CreateTemplateNodeInfoBySelfType();
            result.nodeID = nodeID;
            result.typename = typename;
            result.title = title;
            result.position = Vector2.zero;
            foreach (var (key, value) in inmapping)
            {
                result.inmapping[key] = value.TemplateClone();
            }
            foreach (var (key, value) in outmapping)
            {
                result.outmapping[key] = value.TemplateClone();
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

        [return: IsInstantiated(true)]
        public virtual Node Instantiate()
        {
            //throw new NotImplementedException("Node.Instantiate not implemented");
            string key = this.GetType().Name;
            if (key.EndsWith("Info"))
                key = key[..^4];
            var node = GameObject.Instantiate(WorkflowManager.instance.GraphNodePrefabs.Datas[key].uobjectValue as GameObject).GetComponent<Node>();
            node.SetupFromInfo(this);
            return node;
        }
    }

    public class Node : WindowsComponent, IOnlyFocusThisOnInspector, ITitle
    {
#if UNITY_EDITOR
        [Content]
        public void DebugLogNodeInfo()
        {
            Debug.Log(this.info);
        }

#endif

        private BehaviourContextManager Context;
        [Resources, OnlyNotNullMode, SerializeField] private Text Title;
        [Setting]
        public int SlotHeight = 46, TitleHeight = 50, ExtensionHeight = 0;
        [Resources, SerializeField]
        private PropertiesWindow InSlotPropertiesWindow, OutSlotPropertiesWindow;

        private List<PropertiesWindow.ItemEntry> InSlots, OutSlots;

        internal Dictionary<string, NodeSlot> m_Inmapping = new();
        internal Dictionary<string, NodeSlot> m_Outmapping = new();

        [Resources, SerializeField, OnlyNotNullMode] protected BaseWindowPlane InoutContainerPlane;

        public string title
        {
            get => ((ITitle)this.Title).title;
            set => ((ITitle)this.Title).title = value;
        }

        [Setting, SerializeField] private NodeInfo m_info;
        public NodeInfo info { get => m_info; private set => m_info = value; }

        protected virtual void Start()
        {
            if (Context == null)
                Context = this.GetOrAddComponent<BehaviourContextManager>();
            Context.OnPointerDownEvent = BehaviourContextManager.InitializeContextSingleEvent(Context.OnPointerDownEvent, OnPointerDown);
            Context.OnDragEvent = BehaviourContextManager.InitializeContextSingleEvent(Context.OnDragEvent, OnDrag);
            Context.OnPointerClickEvent = BehaviourContextManager.InitializeContextSingleEvent(Context.OnPointerClickEvent, PointerRightClickAndOpenMenu);
        }

        protected virtual void OnDestroy()
        {
            if (InspectorWindow.instance.GetTarget() == this.info)
            {
                InspectorWindow.instance.ClearWindow();
            }
        }

        public virtual void PointerRightClickAndOpenMenu(PointerEventData pointer)
        {
            if (pointer.button == PointerEventData.InputButton.Right)
            {
                List<SharedModule.CallbackData> callbacks = new()
                {
                    new (WorkflowManager.Transformer("Delete"), x =>
                    {
                        WorkflowManager.instance.DestroyNode(this);
                    })
                };
                SharedModule.instance.OpenCustomMenu(WorkflowManager.instance.UIFocusObject, callbacks.ToArray());
            }
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
            foreach (var info in m_Outmapping)
            {
                if (info.Value != null)
                {
                    foreach (var targetSlot in info.Value.info.targetSlots)
                    {
                        targetSlot.SetDirty();
                    }
                }
            }
        }

        protected virtual void WhenSetup(NodeInfo info)
        {

        }

        public void SetupFromInfo([In] NodeInfo value)
        {
            ClearLink();
            if (HierarchyWindow.instance != null)
            {
                HierarchyWindow.instance.RemoveReference(this.info);
                HierarchyWindow.instance.CreateRootItemEntryWithBinders(value);
            }
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
            RefreshRectTransform();
            WhenSetup(info);
        }

        public void RefreshImmediate()
        {
            //ClearLink();
            //RefreshPosition();
            //BuildLink();
        }
        public void RefreshPosition()
        {
            this.transform.position = info.position;
        }
        public void RefreshRectTransform()
        {
            //InoutContainerPlane.AdjustSizeToContainsChilds();
            this.rectTransform.sizeDelta = new(this.rectTransform.sizeDelta.x, TitleHeight + Mathf.Max(m_Inmapping.Count, m_Outmapping.Count) * SlotHeight + ExtensionHeight);
        }
        public virtual void ClearLink()
        {
            if (InSlotPropertiesWindow == true)
                foreach (var (name, slot) in this.m_Inmapping)
                {
                    NodeSlot.UnlinkAll(slot);
                    slot.SetDirty();
                }
            if (OutSlotPropertiesWindow == true)
                foreach (var (name, slot) in m_Outmapping)
                {
                    NodeSlot.UnlinkAll(slot);
                    slot.SetDirty();
                }
        }
        public virtual void ClearSlots()
        {
            if (this.info == null)
            {
                return;
            }
            if (InSlotPropertiesWindow == true)
            {
                foreach (var slot in this.InSlots)
                    slot.Release();
                InSlots.Clear();
                this.m_Inmapping.Clear();
            }
            if (OutSlotPropertiesWindow == true)
            {
                foreach (var slot in this.OutSlots)
                    slot.Release();
                OutSlots.Clear();
                this.m_Outmapping.Clear();
            }
        }
        protected List<PropertiesWindow.ItemEntry> CreateGraphNodeInSlots(int count)
        {
            if (InSlotPropertiesWindow == null)
                throw new InvalidOperationException($"this node is not using {nameof(InSlotPropertiesWindow)}");
            return InSlotPropertiesWindow.CreateRootItemEntries(count);
        }
        protected List<PropertiesWindow.ItemEntry> CreateGraphNodeOutSlots(int count)
        {
            if (OutSlotPropertiesWindow == null)
                throw new InvalidOperationException($"this node is not using {nameof(OutSlotPropertiesWindow)}");
            return OutSlotPropertiesWindow.CreateRootItemEntries(count);
        }
        public virtual void BuildSlots()
        {
            if (InSlotPropertiesWindow == true)
            {
                int InSlotCount = info.inmapping.Count;
                InSlots = CreateGraphNodeInSlots(InSlotCount);
                foreach (var (key, slotInfo) in info.inmapping)
                {
                    InSlotCount--;
                    var slot = InSlots[InSlotCount].ref_value.GetComponent<NodeSlot>();
                    m_Inmapping.Add(key, slot);
                    var info = slotInfo.TemplateClone();
                    info.parentNode = this;
                    slot.SetupFromInfo(info);
                }
            }
            if (OutSlotPropertiesWindow == true)
            {
                int OutSlotCount = info.outmapping.Count;
                OutSlots = CreateGraphNodeOutSlots(OutSlotCount);
                foreach (var (key, slotInfo) in info.outmapping)
                {
                    OutSlotCount--;
                    var slot = OutSlots[OutSlotCount].ref_value.GetComponent<NodeSlot>();
                    m_Outmapping.Add(key, slot);
                    var info = slotInfo.TemplateClone();
                    info.parentNode = this;
                    slot.SetupFromInfo(info);
                }
            }
            RefreshRectTransform();
        }
        public virtual void BuildLink()
        {
            if (InSlotPropertiesWindow == true)
                foreach (var (slot_name, slot_info) in info.inmapping)
                {
                    var targetNode = WorkflowManager.instance.GetGraphNode(slot_info.targetNodeID);
                    if (targetNode != null)
                    {
                        NodeSlot.Link(m_Inmapping[slot_name], targetNode.m_Outmapping[slot_info.targetSlotName]);
                    }
                    else
                    {
                        NodeSlot.UnlinkAll(m_Inmapping[slot_name]);
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
            NodeSlot.UnlinkAll(this.m_Inmapping[slotName]);
        }
        public void UnlinkOutslot([In] string slotName)
        {
            NodeSlot.UnlinkAll(this.m_Outmapping[slotName]);
        }
    }

    [Serializable, ArgPackage]
    public class DynamicNodeInfo : NodeInfo
    {
        protected override NodeInfo CreateTemplateNodeInfoBySelfType()
        {
            return new DynamicNodeInfo();
        }
    }
}
