using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

namespace Convention.WindowsUI
{
    public class BaseDragBehaviour : MonoBehaviour, IDragHandler, IBehaviourOperator
    {
        public UnityEvent<PointerEventData> OnDragEvent;

        public void OnDrag(PointerEventData eventData)
        {
            OnDragEvent?.Invoke(eventData);
        }
    }
}
