using System.Collections;
using System.Collections.Generic;
using Convention.WindowsUI;
using Unity.VisualScripting.YamlDotNet.Core;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.InputSystem;
using UnityEngine.UI;

namespace Convention.Workflow
{
    public class GraphNodeSlot : WindowsComponent,ITitle
    {
        [Content,OnlyPlayMode] public GraphNode Parent;
        [Resources, OnlyNotNullMode, SerializeField] private Convention.WindowsUI.Text Title;
        [Resources,OnlyNotNullMode,SerializeField]private LineRenderer LineRenderer;
        [Resources, OnlyNotNullMode, SerializeField] private Transform Anchor;
        [Setting] public float Offset = 1;

        public string title { get => ((ITitle)this.Title).title; set => ((ITitle)this.Title).title = value; }

        [Content, OnlyPlayMode] public GraphNodeSlot TargetSlot;

        private void Start()
        {
            BehaviourContextManager contextManager = this.GetOrAddComponent<BehaviourContextManager>();
            contextManager.OnBeginDragEvent = BehaviourContextManager.InitializeContextSingleEvent(contextManager.OnBeginDragEvent, BeginDragLine);
            contextManager.OnDragEvent = BehaviourContextManager.InitializeContextSingleEvent(contextManager.OnBeginDragEvent, DragLine);
            contextManager.OnEndDragEvent = BehaviourContextManager.InitializeContextSingleEvent(contextManager.OnBeginDragEvent, EndDragLine);
        }

        [Content, Ignore, SerializeField] private bool IsKeepDrag = false;
        [Content, Ignore, SerializeField] private bool IsDirty = false;
        private void Update()
        {
            if (IsKeepDrag == false && IsDirty)
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
            if (TargetSlot == null)
            {
                LineRenderer.positionCount = 0;
            }
            else
            {
                LineRenderer.positionCount = 4;
                LineRenderer.SetPosition(0, Anchor.localPosition);

            }
        }

        public void BeginDragLine(PointerEventData _)
        {
            IsKeepDrag = true;
        }
        public void DragLine(PointerEventData pointer)
        {
            LineRenderer.positionCount = 2;
            LineRenderer.SetPosition(0, Anchor.localPosition);
            LineRenderer.SetPosition(1, pointer.pointerCurrentRaycast.worldPosition);
        }
        public void EndDragLine(PointerEventData _)
        {
            IsKeepDrag = false;
        }
    }
}
