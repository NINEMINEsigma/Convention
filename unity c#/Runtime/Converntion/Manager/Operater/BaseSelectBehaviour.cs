using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

namespace Convention.WindowsUI
{
    public class BaseSelectBehaviour : MonoBehaviour, ISelectHandler, IBehaviourOperator
    {
        public UnityEvent<BaseEventData> OnSelectEvent;

        public void OnSelect(BaseEventData eventData)
        {
            OnSelectEvent?.Invoke(eventData);
        }
    }
}
