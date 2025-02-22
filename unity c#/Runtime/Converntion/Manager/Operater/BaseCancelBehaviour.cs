using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

namespace Convention.WindowsUI
{
    public class BaseCancelBehaviour : MonoBehaviour, ICancelHandler, IBehaviourOperator
    {
        public UnityEvent<BaseEventData> OnCancelEvent;

        public void OnCancel(BaseEventData eventData)
        {
            OnCancelEvent?.Invoke(eventData);
        }
    }
}
