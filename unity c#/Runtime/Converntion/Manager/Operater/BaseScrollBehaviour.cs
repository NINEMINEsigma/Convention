using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

namespace Convention.WindowsUI
{
    public class BaseScrollBehaviour : MonoBehaviour, IScrollHandler, IBehaviourOperator
    {
        public UnityEvent<PointerEventData> OnScrollEvent;

        public void OnScroll(PointerEventData eventData)
        {
            OnScrollEvent?.Invoke(eventData);
        }
    }
}
