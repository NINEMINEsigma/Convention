using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

namespace Convention.WindowsUI
{
    public class BaseDeselectBehaviour : MonoBehaviour, IDeselectHandler, IBehaviourOperator
    {
        public UnityEvent<BaseEventData> OnDeselectEvent;

        public void OnDeselect(BaseEventData eventData)
        {
            OnDeselectEvent?.Invoke(eventData);
        }
    }
}
