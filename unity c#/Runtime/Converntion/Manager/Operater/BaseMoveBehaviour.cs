using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

namespace Convention.WindowsUI
{
    public class BaseMoveBehaviour : MonoBehaviour, IMoveHandler, IBehaviourOperator
    {
        public UnityEvent<AxisEventData> OnMoveEvent;

        public void OnMove(AxisEventData eventData)
        {
            OnMoveEvent?.Invoke(eventData);
        }
    }
}
