using System;
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
        /// 目标节点, 对于输入槽而言这是连接的上游输出节点, 对于输出槽而言这是最后一个连接到此的输入节点
        /// </summary>
        [Ignore, NonSerialized, Description("This is a lazy variable that needs to be taken care of manually syncing the value of the " + nameof(targetNodeID))]
        public Node targetNode = null;
        /// <summary>
        /// 目标槽, 对于输入槽而言这是连接的上游输出槽, 对于输出槽而言这是最后一个连接到此的输入槽
        /// </summary>
        [Ignore, NonSerialized, Description("This is a lazy variable that needs to be taken care of manually syncing the value of the " + nameof(targetSlotName))]
        public NodeSlot targetSlot = null;
        /// <summary>
        /// 目标节点ID
        /// </summary>
        public int targetNodeID = -1;
        /// <summary>
        /// 目标插槽名称
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

        public virtual NodeSlotInfo TemplateClone()
        {
            return new()
            {
                slotName = slotName,
                targetNodeID = -1,
                typeIndicator = typeIndicator,
                IsInmappingSlot = IsInmappingSlot
            };
        }
    }

    public interface INodeSlotLinkable
    {
        public bool LinkTo([In, Opt] NodeSlot other);
        public bool Linkable([In]NodeSlot other);
    }

    public class NodeSlot : WindowUIModule, ITitle, INodeSlotLinkable
    {
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
            left.Linkable(right);
            //if (left.info.IsInmappingSlot || left.info.targetSlot == right)
            {
                Unlink(left);
            }
            //if (right.info.IsInmappingSlot || right.info.targetSlot == left)
            {
                Unlink(right);
            }
            left.info.targetSlot = right;
            left.info.targetSlotName = right.info.slotName;
            left.info.targetNode = right.info.parentNode;
            left.info.targetNodeID = WorkflowManager.instance.GetGraphNodeID(right.info.targetNode);
            left.SetDirty();
            right.info.targetSlot = left;
            right.info.targetSlotName = left.info.slotName;
            right.info.targetNode = left.info.parentNode;
            right.info.targetNodeID = WorkflowManager.instance.GetGraphNodeID(left.info.targetNode);
            right.SetDirty();
        }
        public static void Unlink([In] NodeSlot slot)
        {
            var targetSlot = slot.info.targetSlot;
            slot.info.targetSlot = null;
            slot.info.targetNode = null;
            slot.info.targetNodeID = -1;
            if (targetSlot != null && targetSlot.info.targetSlot == slot)
            {
                targetSlot.info.targetSlot = null;
                targetSlot.info.targetNode = null;
                targetSlot.info.targetNodeID = -1;
                targetSlot.SetDirty();
            }
            slot.SetDirty();
        }
        public bool LinkTo([In, Opt] NodeSlot slot)
        {
            if (slot != null)
            {
                Link(this, slot);
                return true;
            }
            else
            {
                Unlink(this);
                return true;
            }
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

        [Content, OnlyPlayMode, Ignore] public NodeSlotInfo info { get; private set; }
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
        [Content, Ignore, SerializeField] private bool OthersideKeepDrag = false;
        private void Update()
        {
            if (IsKeepDrag == false && OthersideKeepDrag == false && IsDirty)
            {
                UpdateLineImmediate();
            }
            else if (IsDirty)
            {
                LineRenderer.SetPositions(Points);
            }
        }

        public void SetDirty()
        {
            IsDirty = true;
        }

        public void UpdateLineImmediate()
        {
            if (info.targetSlot != null && info.IsInmappingSlot)
            {
                Points = new Vector3[]
                {
                    this.Anchor.localPosition,
                    this.Anchor.localPosition+Vector3.left*30,
                    (info.targetSlot.Anchor.position-this.Anchor.position)*ScaleFactor+this.Anchor.localPosition+Vector3.right*30,
                    (info.targetSlot.Anchor.position-this.Anchor.position)*ScaleFactor+this.Anchor.localPosition
                };
            }
            LineRenderer.positionCount = Points.Length;
            title = info.slotName;
            LineRenderer.SetPositions(Points);
            IsDirty = false;
        }

        public void BeginDragLine(PointerEventData _)
        {
            Unlink(this);
            IsKeepDrag = true;
#if UNITY_EDITOR
            if (info == null)
            {
                Points = zeroVecs;
                this.SetDirty();
                return;
            }
#endif
            if (info.targetNode != null)
            {
                info.targetSlot.Points = zeroVecs;
                info.targetSlot.SetDirty();
            }
            Points = zeroVecs;
            SetDirty();
        }
        public void DragLine(PointerEventData pointer)
        {
            LineRenderer.positionCount = 2;
            // Item Canvas具有0.01的缩放, 这里补回来
            Points = new Vector3[] { Anchor.localPosition, (pointer.pointerCurrentRaycast.worldPosition - Anchor.position) * ScaleFactor + Anchor.localPosition };
            SetDirty();
        }
        public void EndDragLine(PointerEventData _)
        {
            IsKeepDrag = false;
            Points = zeroVecs;
            if (CurrentHighLightSlot != null && CurrentHighLightSlot.info.IsInmappingSlot != this.info.IsInmappingSlot)
            {
                Link(this, CurrentHighLightSlot);
            }
            SetDirty();
            DisableAllHighLight();
        }
    }
}
