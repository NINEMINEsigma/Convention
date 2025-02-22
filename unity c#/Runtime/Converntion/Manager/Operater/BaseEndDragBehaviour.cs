using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

namespace Convention.WindowsUI
{
    public class BaseEndDragBehaviour : MonoBehaviour, IEndDragHandler, IBehaviourOperator
    {
        public UnityEvent<PointerEventData> OnEndDragEvent;

        public void OnEndDrag(PointerEventData eventData)
        {
            OnEndDragEvent?.Invoke(eventData);
        }
    }
}
