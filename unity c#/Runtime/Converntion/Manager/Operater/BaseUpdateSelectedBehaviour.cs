using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

namespace Convention.WindowsUI
{
    public class BaseUpdateSelectedBehaviour : MonoBehaviour, IUpdateSelectedHandler, IBehaviourOperator
    {
        public UnityEvent<BaseEventData> OnUpdateSelectedEvent;

        public void OnUpdateSelected(BaseEventData eventData)
        {
            OnUpdateSelectedEvent?.Invoke(eventData);
        }
    }
}
