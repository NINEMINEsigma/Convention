using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

namespace Convention.WindowsUI
{
    public class BaseSubmitBehaviour : MonoBehaviour, ISubmitHandler, IBehaviourOperator
    {
        public UnityEvent<BaseEventData> OnSubmitEvent;

        public void OnSubmit(BaseEventData eventData)
        {
            OnSubmitEvent?.Invoke(eventData);
        }
    }
}
