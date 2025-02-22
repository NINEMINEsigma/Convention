using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

namespace Convention.WindowsUI
{
    public class BasePointerClickBehaviour : MonoBehaviour, IPointerClickHandler, IBehaviourOperator
    {
        public UnityEvent<PointerEventData> OnPointerClickEvent;

        public void OnPointerClick(PointerEventData eventData)
        {
            OnPointerClickEvent?.Invoke(eventData);
        }
    }
}
