using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

namespace Convention.WindowsUI
{
    public class BaseInitializePotentialDragBehaviour : MonoBehaviour, IInitializePotentialDragHandler, IBehaviourOperator
    {
        public UnityEvent<PointerEventData> OnInitializePotentialDragEvent;

        public void OnInitializePotentialDrag(PointerEventData eventData)
        {
            OnInitializePotentialDragEvent?.Invoke(eventData);
        }
    }
}
