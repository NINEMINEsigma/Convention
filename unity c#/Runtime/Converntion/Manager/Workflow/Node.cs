using System;
using System.Collections.Generic;
using System.IO;
using Convention.WindowsUI;
using Convention.WindowsUI.Variant;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.InputSystem;

namespace Convention.Workflow
{
    [Serializable, ArgPackage]
    public class NodeInfo : AnyClass,IHierarchyItemTitle
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
            get => this.title;
            set => this.title = node.title = value;
        }

        string IHierarchyItemTitle.HierarchyItemTitle => title;

        public NodeInfo()
        {
            WorkflowManager.Transformer(typename = this.GetType().Name[..^4]);
        }

        protected virtual NodeInfo CreateTemplate()
        {
            return new NodeInfo();
        }
        protected virtual void CloneValues([In] NodeInfo clonen)
        {
            clonen.nodeID = nodeID;
            clonen.typename = typename;
            clonen.title = string.IsNullOrEmpty(title) ? WorkflowManager.Transformer(this.GetType().Name[..^4]) : title;
            clonen.position = position;
            foreach (var (key, value) in inmapping)
            {
                clonen.inmapping[key] = value.TemplateClone();
            }
            foreach (var (key, value) in outmapping)
            {
                clonen.outmapping[key] = value.TemplateClone();
            }
        }
        public NodeInfo TemplateClone()
        {
            NodeInfo result = CreateTemplate();
            CloneValues(result);
            return result;
        }

        public static Vector2 GetPosition(Transform transform)
        {
            Vector3 result = transform.position - WorkflowManager.instance.ContentPlane.transform.position;
            return new(result.x, result.y);
        }

        public virtual void CopyFromNode([In] Node node)
        {
            nodeID = WorkflowManager.instance.GetGraphNodeID(node);
            title = node.title;
            position = GetPosition(node.transform);
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
            string key = this.GetType().Name;
            if (key.EndsWith("Info"))
                key = key[..^4];
            var node = GameObject.Instantiate(WorkflowManager.instance.GraphNodePrefabs.FindItem<GameObject>(key)).GetComponent<Node>();
            return node;
        }

        public override string ToString()
        {
            return $"{title}<in.c={(inmapping == null ? 0 : inmapping.Count)}, out.c={(outmapping == null ? 0 : outmapping.Count)}>";
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

        public PropertiesWindow.ItemEntry MyNodeTab; 

        private BehaviourContextManager Context;
        [Resources, OnlyNotNullMode, SerializeField] private Text Title;
        [Setting]
        public int SlotHeight = 40, TitleHeight = 50, ExtensionHeight = 0;

        public bool IsStartNode => this.GetType().IsSubclassOf(typeof(StartNode));
        public bool IsEndNode => this.GetType().IsSubclassOf(typeof(EndNode));

        [Resources, SerializeField, WhenAttribute.Is(nameof(IsStartNode), false), OnlyNotNullMode]
        private PropertiesWindow InSlotPropertiesWindow;
        [Resources, SerializeField, WhenAttribute.Is(nameof(IsEndNode), false), OnlyNotNullMode]
        private PropertiesWindow OutSlotPropertiesWindow;

        private List<PropertiesWindow.ItemEntry> InSlots = new(), OutSlots = new();

        internal Dictionary<string, NodeSlot> m_Inmapping = new();
        internal Dictionary<string, NodeSlot> m_Outmapping = new();

        [Resources, SerializeField, OnlyNotNullMode] protected BaseWindowPlane InoutContainerPlane;

        [Content, OnlyPlayMode, SerializeField] private string rawTitle;
        public string title
        {
            get => rawTitle;
            set
            {
                rawTitle = value;
                this.Title.title = WorkflowManager.Transformer(rawTitle);
            }
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
            Context.OnEndDragEvent = BehaviourContextManager.InitializeContextSingleEvent(Context.OnEndDragEvent, EndDrag);
        }

        protected virtual void OnDestroy()
        {
            if (InspectorWindow.instance.GetTarget() == this.info)
            {
                InspectorWindow.instance.ClearWindow();
            }
            MyNodeTab?.Release();
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
            RefreshImmediate();
        }

        public void EndDrag(PointerEventData _)
        {
            if (Keyboard.current[Key.LeftCtrl].isPressed)
            {
                var vec = this.transform.localPosition;
                float x = Mathf.Round(vec.x / 100);
                float y = Mathf.Round(vec.y / 100);
                transform.localPosition = new Vector3(x * 100, y * 100, 0);
                RefreshImmediate();
            }
        }

        protected virtual void WhenSetup(NodeInfo info)
        {

        }

        public void SetupFromInfo([In] NodeInfo value, bool isRefresh = true)
        {
            if (value != info)
            {
                ClearLink();
                ClearSlots();
                info = value;
                this.title = value.title;
                int nodeID = WorkflowManager.instance.GetGraphNodeID(this);
                value.nodeID = nodeID;
                value.node = this;
                if (isRefresh)
                {
                    BuildSlots();
                    BuildLink();
                    RefreshPosition();
                }
                WhenSetup(info);
            }
        }

        public void RefreshImmediate()
        {
            foreach (var (_, slot) in m_Inmapping)
            {
                if (slot != null)
                    slot.SetDirty();
            }
            foreach (var (_, slot) in m_Outmapping)
            {
                if (slot != null)
                {
                    foreach (var targetSlot in slot.info.targetSlots)
                    {
                        targetSlot.SetDirty();
                    }
                }
            }
        }
        public void RefreshPosition()
        {
            this.transform.position = new Vector3(info.position.x, info.position.y) + (WorkflowManager.instance.ContentPlane.transform.position);
        }
        public void RefreshRectTransform()
        {
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
                    // 这样真的好吗
                    m_Inmapping[key] = slot;
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
                    // 通过这种方法规避了重复键, 但是也存在一些特殊的问题
                    m_Outmapping[key] = slot;
                    var info = slotInfo.TemplateClone();
                    info.parentNode = this;
                    slot.SetupFromInfo(info);
                }
            }
            RefreshRectTransform();
        }
        public virtual void BuildLink()
        {
            if (InSlotPropertiesWindow != null)
            {
                foreach (var (slot_name, slot_info) in info.inmapping)
                {
                    var targetNode = WorkflowManager.instance.GetGraphNode(slot_info.targetNodeID);
                    if (targetNode != null)
                    {
                        NodeSlot.Link(m_Inmapping[slot_name], targetNode.m_Outmapping[slot_info.targetSlotName]);
                    }
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
        protected override NodeInfo CreateTemplate()
        {
            return new DynamicNodeInfo();
        }
    }
}
