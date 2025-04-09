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
        /// �����ĸ��ڵ�
        /// </summary>
        [Ignore, NonSerialized] public Node parentNode = null;
        /// <summary>
        /// �����Ĳ��
        /// </summary>
        [Ignore, NonSerialized] public NodeSlot slot = null;
        /// <summary>
        /// �������
        /// </summary>
        public string slotName = "unknown";
        /// <summary>
        /// Ŀ��ڵ�, ��������۶����������ӵ���������ڵ�, ��������۶����������һ�����ӵ��˵�����ڵ�
        /// </summary>
        [Ignore, NonSerialized, Description("This is a lazy variable that needs to be taken care of manually syncing the value of the " + nameof(targetNodeID))]
        public Node targetNode = null;
        /// <summary>
        /// Ŀ���, ��������۶����������ӵ����������, ��������۶����������һ�����ӵ��˵������
        /// </summary>
        [Ignore, NonSerialized, Description("This is a lazy variable that needs to be taken care of manually syncing the value of the " + nameof(targetSlotName))]
        public NodeSlot targetSlot = null;
        /// <summary>
        /// Ŀ��ڵ�ID
        /// </summary>
        public int targetNodeID = -1;
        /// <summary>
        /// Ŀ��������
        /// </summary>
        public string targetSlotName = "unknown";
        /// <summary>
        /// ����ָʾ��
        /// </summary>
        public string typeIndicator;
        /// <summary>
        /// �Ƿ�Ϊ����ӳ����
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

    public class NodeSlot : WindowUIModule, ITitle
    {
        public static void Link([In]NodeSlot left,[In] NodeSlot right)
        {
            if(left.info.IsInmappingSlot==right.info.IsInmappingSlot)
            {
                throw new InvalidOperationException($"{left} and {right} has same mapping type");
            }
            if (left.info.typeIndicator != right.info.typeIndicator)
            {
                throw new InvalidOperationException($"{left} and {right} has different type indicator");
            }
            if (left.info.parentNode == right.info.parentNode)
            {
                throw new InvalidOperationException($"{left} and {right} has same parent node");
            }
            if (left.info.IsInmappingSlot || left.info.targetSlot == right)
            {
                Unlink(left);
                left.info.targetSlot = right;
                left.info.targetSlotName = right.info.slotName;
                left.info.targetNode = right.info.parentNode;
                left.info.targetNodeID = WorkflowManager.instance.GetGraphNodeID(right.info.targetNode);
                left.SetDirty();
            }
            if (left.info.IsInmappingSlot && left.info.targetSlot == right)
            {
                Unlink(right);
                right.info.targetSlot = left;
                right.info.targetSlotName = left.info.slotName;
                right.info.targetNode = right.info.parentNode;
                right.info.targetNodeID = WorkflowManager.instance.GetGraphNodeID(left.info.targetNode);
                right.SetDirty();
            }
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
        public void LinkTo([In,Opt]NodeSlot slot)
        {
            if (slot != null)
                Link(this, slot);
            else
                Unlink(this);
        }

        public static NodeSlot CurrentHighLightSlot { get; private set; }
        public static void EnableHighLight(NodeSlot slot)
        {
            if (CurrentHighLightSlot != null)
            {
                CurrentHighLightSlot.HighLight.SetActive(false);
            }
            CurrentHighLightSlot = slot;
            CurrentHighLightSlot.HighLight.SetActive(true);
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
            }
        }

        public static readonly Vector3[] zeroVecs = new Vector3[0];

        [Content, OnlyPlayMode, Ignore] public NodeSlotInfo info { get;private set; }
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
            else if(IsDirty)
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
            LineRenderer.positionCount = Points.Length;
#if UNITY_EDITOR
            if (info != null)
#endif
            {
                title = info.slotName;
            }
            LineRenderer.SetPositions(Points);
            IsDirty = false;
        }

        public void BeginDragLine(PointerEventData _)
        {
            IsKeepDrag = true;
#if UNITY_EDITOR
            if(info==null)
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
            // ��������Ǵ��
            Points = new Vector3[] { Anchor.localPosition, pointer.pointerCurrentRaycast.worldPosition - Anchor.position + Anchor.localPosition };
            SetDirty();
        }
        public void EndDragLine(PointerEventData _)
        {
            IsKeepDrag = false;
            if (CurrentHighLightSlot != null && CurrentHighLightSlot.info.IsInmappingSlot != this.info.IsInmappingSlot)
            {
                Link(this, CurrentHighLightSlot);
            }
            DisableAllHighLight();
        }
    }
}
