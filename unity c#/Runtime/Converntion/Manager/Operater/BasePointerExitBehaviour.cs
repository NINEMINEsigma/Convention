using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

namespace Convention.WindowsUI
{
    public class BasePointerExitBehaviour : MonoBehaviour, IPointerExitHandler, IBehaviourOperator
    {
        public UnityEvent<PointerEventData> OnPointerExitEvent;

        public void OnPointerExit(PointerEventData eventData)
        {
            OnPointerExitEvent?.Invoke(eventData);
        }
    }
}
