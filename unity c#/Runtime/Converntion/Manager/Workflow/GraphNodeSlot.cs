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
        public string slotName;

        [Ignore, NonSerialized, Description("This is a lazy variable that needs to be taken care of manually syncing the value of the " + nameof(targetNodeID))]
        public GraphNode targetNode;
        [Ignore, NonSerialized, Description("This is a lazy variable that needs to be taken care of manually syncing the value of the " + nameof(targetSlotName))]
        public GraphNodeSlot targetSlot;
        public int targetNodeID;
        public string targetSlotName;

        public string typeIndicator;
        public bool IsInmappingSlot;

        public void CopyFromNode()
        {

        }

    }

    public class GraphNodeSlot : WindowsComponent, ITitle
    {
        private static void Link(GraphNodeSlot left, GraphNodeSlot right)
        {
            if(left.Info.IsInmappingSlot==right.Info.IsInmappingSlot)
            {
                throw new InvalidOperationException($"{left} and {right} has same mapping type");
            }
            if (left.Info.typeIndicator != right.Info.typeIndicator)
            {
                throw new InvalidOperationException($"{left} and {right} has different type indicator");
            }
            left.Info.targetSlot = right;
            right.Info.targetSlot = left;

            left.Info.targetSlotName = right.Info.slotName;
            right.Info.targetSlotName = left.Info.slotName;

            left.Info.targetNode = right.Info.parentNode;
            right.Info.targetNode = right.Info.parentNode;

            left.Info.targetNodeID = WorkflowManager.instance.GetGraphNodeID(right.Info.targetNode);
            right.Info.targetNodeID = WorkflowManager.instance.GetGraphNodeID(left.Info.targetNode);

            left.SetDirty();
            right.SetDirty();
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

        [Content, OnlyPlayMode, Ignore, SerializeField] private GraphNodeSlotInfo m_info;
        public GraphNodeSlotInfo Info
        {
            get => m_info;
            set
            {
                if (m_info != value)
                {
                    m_info = value;
                    SetDirty();
                }
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
                (CurrentHighLightSlot.Info.IsInmappingSlot != this.Info.IsInmappingSlot &&
                CurrentHighLightSlot.Info.typeIndicator == this.Info.typeIndicator)
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
            LineRenderer.SetPositions(Points);
        }

        public void BeginDragLine(PointerEventData _)
        {
            IsKeepDrag = true;
            if (Info.targetNode != null)
            {
                Info.targetSlot.Points = zeroVecs;
                Info.targetSlot.SetDirty();
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
            if (CurrentHighLightSlot != null && CurrentHighLightSlot.Info.IsInmappingSlot != this.Info.IsInmappingSlot)
            {
                Link(this, CurrentHighLightSlot);
            }
            DisableAllHighLight();
        }
    }
}
