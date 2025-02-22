using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

namespace Convention.WindowsUI
{
    public class BasePointerDownBehaviour : MonoBehaviour, IPointerDownHandler, IBehaviourOperator
    {
        public UnityEvent<PointerEventData> OnPointerDownEvent;

        public void OnPointerDown(PointerEventData eventData)
        {
            OnPointerDownEvent?.Invoke(eventData);
        }
    }
}
