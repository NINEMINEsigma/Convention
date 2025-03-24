using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Controls;

namespace Convention.WindowsUI
{
    public class ClickBoard : WindowUIModule
    {
        [Resources] public BehaviourContextManager Context;

        [Setting] public UnityEvent<PointerEventData> LeftButtonClick = new();
        [Setting] public UnityEvent<PointerEventData> RightButtonClick = new();

        private void Start()
        {
            if (Context == null)
                Context = this.GetOrAddComponent<BehaviourContextManager>();

            Context.OnPointerClickEvent = BehaviourContextManager.InitializeContextSingleEvent(Context.OnPointerClickEvent, point =>
            {
                if (point.button == PointerEventData.InputButton.Left)
                {
                    LeftButtonClick.Invoke(point);
                }
                if (point.button == PointerEventData.InputButton.Right)
                {
                    RightButtonClick.Invoke(point);
                }
            });
        }
    }
}
