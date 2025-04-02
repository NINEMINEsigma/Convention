using System;
using System.Security.Cryptography;
using Convention.WindowsUI;
using UnityEngine;
using UnityEngine.EventSystems;

namespace Convention.Workflow
{
    [Serializable, ArgPackage]
    public class GraphNodeSlotInfo
    {
        [Ignore, NonSerialized] public GraphNode parentNode;
        [Ignore, NonSerialized] public GraphNodeSlot slot;
        public string slotName;

        [Ignore, NonSerialized, Description("This is a lazy variable that needs to be taken care of manually syncing the value of the " + nameof(targetNodeID))]
        public GraphNode targetNode;
        [Ignore, NonSerialized, Description("This is a lazy variable that needs to be taken care of manually syncing the value of the " + nameof(targetSlotName))]
        public GraphNodeSlot targetSlot;
        public int targetNodeID;
        public string targetSlotName;

        public string typeIndicator;
        public bool IsInmappingSlot;

        public virtual GraphNodeSlotInfo TemplateClone()
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

    public class GraphNodeSlot : WindowUIModule, ITitle
    {
        public static void Link(GraphNodeSlot left, GraphNodeSlot right)
        {
            if(left.info.IsInmappingSlot==right.info.IsInmappingSlot)
            {
                throw new InvalidOperationException($"{left} and {right} has same mapping type");
            }
            if (left.info.typeIndicator != right.info.typeIndicator)
            {
                throw new InvalidOperationException($"{left} and {right} has different type indicator");
            }
            if (left.info.targetNodeID == right.info.targetNodeID)
            {
                throw new InvalidOperationException($"{left} and {right} has same target node id");
            }
            left.info.targetSlot = right;
            right.info.targetSlot = left;

            left.info.targetSlotName = right.info.slotName;
            right.info.targetSlotName = left.info.slotName;

            left.info.targetNode = right.info.parentNode;
            right.info.targetNode = right.info.parentNode;

            left.info.targetNodeID = WorkflowManager.instance.GetGraphNodeID(right.info.targetNode);
            right.info.targetNodeID = WorkflowManager.instance.GetGraphNodeID(left.info.targetNode);

            left.SetDirty();
            right.SetDirty();
        }
        public static void Unlink(GraphNodeSlot slot)
        {
            var targetSlot = slot.info.targetSlot;
            slot.info.targetSlot = null;
            slot.info.targetNode = null;
            slot.info.targetNodeID = -1;
            if(targetSlot != null)
            {
                targetSlot.info.targetSlot = null; 
                targetSlot.info.targetNode = null;
                targetSlot.info.targetNodeID = -1;
                targetSlot.SetDirty();
            }
            slot.SetDirty();
        }

        public static GraphNodeSlot CurrentHighLightSlot { get; private set; }
        public static void EnableHighLight(GraphNodeSlot slot)
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
        public static void DisableHighLight(GraphNodeSlot slot)
        {
            if (CurrentHighLightSlot == slot)
            {
                CurrentHighLightSlot.HighLight.SetActive(false);
                CurrentHighLightSlot = null;
            }
        }

        public static readonly Vector3[] zeroVecs = new Vector3[0];

        [Content, OnlyPlayMode, Ignore] public GraphNodeSlotInfo info { get;private set; }
        public void SetupFromInfo(GraphNodeSlotInfo value)
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
                IsDirty = false;
            }
        }

        public void SetDirty()
        {
            IsDirty = true;
        }

        public void UpdateLineImmediate()
        {
            LineRenderer.positionCount = Points.Length;
            title = info.slotName;
            LineRenderer.SetPositions(Points);
        }

        public void BeginDragLine(PointerEventData _)
        {
            IsKeepDrag = true;
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
