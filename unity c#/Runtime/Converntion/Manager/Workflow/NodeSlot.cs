using System;
using System.Collections.Generic;
using Convention.WindowsUI;
using UnityEngine;
using UnityEngine.EventSystems;

namespace Convention.Workflow
{
    [Serializable, ArgPackage]
    public class NodeSlotInfo : AnyClass
    {
        /// <summary>
        /// 所属的父节点
        /// </summary>
        [Ignore, NonSerialized] public Node parentNode = null;
        /// <summary>
        /// 所属的插槽
        /// </summary>
        [Ignore, NonSerialized] public NodeSlot slot = null;
        /// <summary>
        /// 插槽名称
        /// </summary>
        public string slotName = "unknown";
        /// <summary>
        /// 目标节点
        /// </summary>
        [Ignore, NonSerialized]
        public List<Node> targetNodes = new();
        /// <summary>
        /// 目标槽
        /// </summary>
        [Ignore, NonSerialized]
        public List<NodeSlot> targetSlots = new();
        /// <summary>
        /// 目标节点ID, 输出节点无效
        /// </summary>
        public int targetNodeID = -1;
        /// <summary>
        /// 目标插槽名称, 输出节点无效
        /// </summary>
        public string targetSlotName = "unknown";
        /// <summary>
        /// 类型指示器
        /// </summary>
        public string typeIndicator;
        /// <summary>
        /// 是否为输入映射插槽
        /// </summary>
        public bool IsInmappingSlot;

        public NodeSlotInfo TemplateClone(bool isClearn = true)
        {
            NodeSlotInfo result = new()
            {
                slotName = slotName,
                targetNodeID = isClearn ? -1 : this.targetNodeID,
                targetSlotName = isClearn ? "" : this.targetSlotName,
                typeIndicator = typeIndicator,
                IsInmappingSlot = IsInmappingSlot
            };
            return result;
        }
    }
    
    public interface INodeSlotLinkable
    {
        public bool LinkTo([In, Opt] NodeSlot other);
        public bool Linkable([In]NodeSlot other);
    }

    public class NodeSlot : WindowUIModule, ITitle, INodeSlotLinkable
    {
        [Resources, Setting, Tooltip("挂载额外的组件")] public GameObject ExtensionModule;

        //这个缩放因子是最顶层Canvas的变形
        public const float ScaleFactor = 100;

        public bool Linkable([In] NodeSlot other)
        {
            if (this.info.IsInmappingSlot == other.info.IsInmappingSlot)
            {
                throw new InvalidOperationException($"{this} and {other} has same mapping type");
            }
            if (this.info.typeIndicator != other.info.typeIndicator)
            {
                if (!((this.info.typeIndicator == "string" && other.info.typeIndicator == "str") ||
                    (this.info.typeIndicator == "str" && other.info.typeIndicator == "string") ||
                    this.info.typeIndicator.StartsWith("Any", StringComparison.CurrentCultureIgnoreCase) ||
                    other.info.typeIndicator.StartsWith("Any", StringComparison.CurrentCultureIgnoreCase
                    )))
                    throw new InvalidOperationException($"{this}<{this.info.typeIndicator}> and {other}<{other.info.typeIndicator}> has different type indicator");
            }
            if (this.info.parentNode == other.info.parentNode)
            {
                throw new InvalidOperationException($"{this} and {other} has same parent node<{this.info.parentNode}>");
            }
            return true;
        }
        public static void Link([In] NodeSlot left, [In] NodeSlot right)
        {
            if(left.info.IsInmappingSlot)
            {
                (left, right) = (right, left);
            }
            left.Linkable(right);
            // left一定是输出端
            if (left.info.targetSlots.Contains(right) == false)
            {
                UnlinkAll(right);

                left.info.targetSlots.Add(right);
                left.info.targetSlotName = right.info.slotName;
                left.info.targetNodes.Add(right.info.parentNode);
                left.info.targetNodeID = WorkflowManager.instance.GetGraphNodeID(right.info.parentNode);

                right.info.targetSlots.Clear();
                right.info.targetSlots.Add(left);
                right.info.targetNodes.Clear();
                right.info.targetNodes.Add(left.info.parentNode);
                right.info.targetNodeID = WorkflowManager.instance.GetGraphNodeID(left.info.parentNode);
                right.info.targetSlotName = left.info.slotName;

                left.SetDirty();
                right.SetDirty();
            }
        }
        public static void Unlink([In] NodeSlot slot, NodeSlot targetSlot)
        {
            int index = slot.info.targetSlots.IndexOf(targetSlot);
            if (index != -1)
                Unlink(slot, index);
        }
        public static void Unlink([In] NodeSlot slot, int slotIndex)
        {
            var targetSlot = slot.info.targetSlots[slotIndex];
            slot.info.targetSlots.RemoveAt(slotIndex);
            slot.info.targetNodes.RemoveAt(slotIndex);
            if (slot.info.targetSlots.Count == 0)
            {
                slot.info.targetNodeID = -1;
                slot.info.targetSlotName = "";
            }
            int r_slotIndex = targetSlot.info.targetSlots.IndexOf(slot);
            if (targetSlot != null && r_slotIndex != -1)
            {
                targetSlot.info.targetSlots.RemoveAt(r_slotIndex);
                targetSlot.info.targetNodes.RemoveAt(r_slotIndex);
                if (targetSlot.info.targetSlots.Count == 0)
                {
                    targetSlot.info.targetNodeID = -1;
                    targetSlot.info.targetSlotName = "";
                }
                targetSlot.SetDirty();
            }
            slot.SetDirty();
        }
        public static void UnlinkAll([In] NodeSlot slot)
        {
            foreach (var otherSlot in slot.info.targetSlots)
            {
                int index = otherSlot.info.targetSlots.IndexOf(slot);
                otherSlot.info.targetSlots.RemoveAt(index);
                otherSlot.info.targetNodes.RemoveAt(index);
                if (otherSlot.info.targetSlots.Count == 0)
                {
                    otherSlot.info.targetNodeID = -1;
                    otherSlot.info.targetSlotName = "";
                }
                otherSlot.SetDirty();
            }
            slot.info.targetSlots.Clear();
            slot.info.targetNodes.Clear();
            slot.info.targetNodeID = -1;
            slot.info.targetSlotName = "";
            slot.SetDirty();
        }
        public bool LinkTo([In, Opt] NodeSlot slot)
        {
            if (slot != null)
            {
                Link(this, slot);
                return true;
            }
            return false;
        }

        public static NodeSlot CurrentHighLightSlot { get; private set; }
        public static INodeSlotLinkable CurrentLinkTarget;
        public static void EnableHighLight(NodeSlot slot)
        {
            if (CurrentHighLightSlot != null)
            {
                CurrentHighLightSlot.HighLight.SetActive(false);
            }
            CurrentHighLightSlot = slot;
            CurrentHighLightSlot.HighLight.SetActive(true);
            CurrentLinkTarget = slot;
        }
        public static void DisableAllHighLight()
        {
            if (CurrentHighLightSlot != null)
            {
                CurrentHighLightSlot.HighLight.SetActive(false);
                CurrentHighLightSlot = null;
            }
        }
        public static void DisableHighLight(NodeSlot slot)
        {
            if (CurrentHighLightSlot == slot)
            {
                CurrentHighLightSlot.HighLight.SetActive(false);
                CurrentHighLightSlot = null;
                CurrentLinkTarget = null;
            }
        }

        public static readonly Vector3[] zeroVecs = new Vector3[0];
        public Vector3[] GetCurrentLinkingVectors()
        {
            if (this.info == null)
                return zeroVecs;
            if (info.targetSlots.Count > 0 && info.IsInmappingSlot)
            {
                Vector3 position = this.Anchor.position;
                Vector3 localPosition = this.Anchor.localPosition;
                Vector3 targetPosition = info.targetSlots[0].Anchor.position;
                float offset = Mathf.Clamp((targetPosition - position).magnitude * ScaleFactor, 0, 30);
                return new Vector3[]{
                        localPosition,
                        localPosition+Vector3.left*offset,
                        (targetPosition-position)*ScaleFactor+ localPosition+Vector3.right*offset,
                        (targetPosition-position)*ScaleFactor+ localPosition
                };
            }
            else return zeroVecs;
        }

        [Content, OnlyPlayMode, Ignore] private NodeSlotInfo m_info;
        public NodeSlotInfo info { get => m_info; private set => m_info = value; }
        public void SetupFromInfo(NodeSlotInfo value)
        {
            if (info != value)
            {
                info = value;
                info.slot = this;
                SetDirty();
            }
        }
        [Resources, OnlyNotNullMode, SerializeField] private Text Title;
        [Resources, OnlyNotNullMode, SerializeField] private LineRenderer LineRenderer;
        [Resources, OnlyNotNullMode, SerializeField] private Transform Anchor;
        [Resources, OnlyNotNullMode, SerializeField] private GameObject HighLight;
        [Setting] public float Offset = 1;
        [Content, SerializeField] private Vector3[] Points = new Vector3[0];

        public string title { get => ((ITitle)this.Title).title; set => ((ITitle)this.Title).title = value; }

        private void OnDestroy()
        {
            UnlinkAll(this);
        }

        private void Start()
        {
            BehaviourContextManager contextManager = this.GetOrAddComponent<BehaviourContextManager>();
            contextManager.OnBeginDragEvent = BehaviourContextManager.InitializeContextSingleEvent(contextManager.OnBeginDragEvent, BeginDragLine);
            contextManager.OnDragEvent = BehaviourContextManager.InitializeContextSingleEvent(contextManager.OnDragEvent, DragLine);
            contextManager.OnEndDragEvent = BehaviourContextManager.InitializeContextSingleEvent(contextManager.OnEndDragEvent, EndDragLine);
            contextManager.OnPointerEnterEvent = BehaviourContextManager.InitializeContextSingleEvent(contextManager.OnPointerEnterEvent, _ =>
            {
                if (
                CurrentHighLightSlot == null ||
                (CurrentHighLightSlot.info.IsInmappingSlot != this.info.IsInmappingSlot &&
                CurrentHighLightSlot.info.typeIndicator == this.info.typeIndicator)
                )
                    EnableHighLight(this);
            });
            contextManager.OnPointerExitEvent = BehaviourContextManager.InitializeContextSingleEvent(contextManager.OnPointerExitEvent, _ =>
            {
                DisableHighLight(this);
            });
        }

        [Content, Ignore, SerializeField] private bool IsKeepDrag = false;
        [Content, Ignore, SerializeField] private bool IsDirty = false;
        private void Update()
        {
            if (IsKeepDrag == false && IsDirty)
            {
                Points = GetCurrentLinkingVectors();
                UpdateLineImmediate();
            }
            else if (IsDirty)
            {
                RebuildLine();
            }
        }

        public void SetDirty()
        {
            IsDirty = true;
        }

        public void RebuildLine()
        {
            LineRenderer.positionCount = Points.Length;
            LineRenderer.SetPositions(Points);
        }

        public void UpdateLineImmediate()
        {
            RebuildLine();
            title = $"{WorkflowManager.Transformer(info.slotName)}({WorkflowManager.Transformer(info.typeIndicator)})";
            IsDirty = false;
        }

        public void BeginDragLine(PointerEventData _)
        {
            if (this.info.IsInmappingSlot)
                UnlinkAll(this);
            IsKeepDrag = true;
            SetDirty();
        }
        public void DragLine(PointerEventData pointer)
        {
            Points = new Vector3[] { Anchor.localPosition, (pointer.pointerCurrentRaycast.worldPosition - Anchor.position) * ScaleFactor + Anchor.localPosition };
            SetDirty();
        }
        public void EndDragLine(PointerEventData _)
        {
            IsKeepDrag = false;
            if (CurrentHighLightSlot != null && CurrentHighLightSlot.info.IsInmappingSlot != this.info.IsInmappingSlot)
            {
                Link(this, CurrentHighLightSlot);
            }
            SetDirty();
            DisableAllHighLight();
        }
    }
}
