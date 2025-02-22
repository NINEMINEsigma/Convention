using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

namespace Convention.WindowsUI
{
    public class BasePointerUpBehaviour : MonoBehaviour, IPointerUpHandler, IBehaviourOperator
    {
        public UnityEvent<PointerEventData> OnPointerUpEvent;

        public void OnPointerUp(PointerEventData eventData)
        {
            OnPointerUpEvent?.Invoke(eventData);
        }
    }
}
