using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

namespace Convention.WindowsUI
{
    public class BaseDropBehaviour : MonoBehaviour, IDropHandler, IBehaviourOperator
    {
        public UnityEvent<PointerEventData> OnDropEvent;

        public void OnDrop(PointerEventData eventData)
        {
            OnDropEvent?.Invoke(eventData);
        }
    }
}
