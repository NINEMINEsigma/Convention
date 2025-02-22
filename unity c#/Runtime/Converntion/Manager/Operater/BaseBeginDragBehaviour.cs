using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

namespace Convention.WindowsUI
{
    public class BaseBeginDragBehaviour : MonoBehaviour, IBeginDragHandler, IBehaviourOperator
    {
        public UnityEvent<PointerEventData> OnBeginDragEvent;

        public void OnBeginDrag(PointerEventData eventData)
        {
            OnBeginDragEvent?.Invoke(eventData);
        }
    }
}
