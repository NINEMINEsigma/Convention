using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

namespace Convention.WindowsUI
{
    public class BasePointerEnterBehaviour : MonoBehaviour, IPointerEnterHandler, IBehaviourOperator
    {
        public UnityEvent<PointerEventData> OnPointerEnterEvent;

        public void OnPointerEnter(PointerEventData eventData)
        {
            OnPointerEnterEvent?.Invoke(eventData);
        }
    }
}
