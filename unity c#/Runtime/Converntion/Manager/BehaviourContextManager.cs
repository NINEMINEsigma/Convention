using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

namespace Convention.WindowsUI
{
    public interface IBehaviourOperator { }

    /// <summary>
    /// 禁止在Awake时刻使用BehaviourContext
    /// </summary>
    public class BehaviourContextManager : MonoAnyBehaviour, ICanvasRaycastFilter
    {
        public static UnityEvent<PointerEventData> InitializeContextSingleEvent(UnityEvent<PointerEventData> Event, params UnityAction<PointerEventData>[] calls)
        {
            Event ??= new();
            foreach (var call in calls)
                Event.RemoveListener(call);
            foreach (var call in calls)
                Event.AddListener(call);
            return Event;
        }
        public static UnityEvent<BaseEventData> InitializeContextSingleEvent(UnityEvent<BaseEventData> Event, params UnityAction<BaseEventData>[] calls)
        {
            Event ??= new();
            foreach (var call in calls)
                Event.RemoveListener(call);
            foreach (var call in calls)
                Event.AddListener(call);
            return Event;
        }
        public static UnityEvent<AxisEventData> InitializeContextSingleEvent(UnityEvent<AxisEventData> Event, params UnityAction<AxisEventData>[] calls)
        {
            Event ??= new();
            foreach (var call in calls)
                Event.RemoveListener(call);
            foreach (var call in calls)
                Event.AddListener(call);
            return Event;
        }
        [Setting]
        public UnityEvent<PointerEventData> OnBeginDragEvent
        {
            get
            {
                if (!TryGetComponent<BaseBeginDragBehaviour>(out var cat)) return null;
                return cat.OnBeginDragEvent;
            }
            set
            {
                var cat = this.GetOrAddComponent<BaseBeginDragBehaviour>();
                cat.OnBeginDragEvent = value;
            }
        }
        [Setting]
        public UnityEvent<PointerEventData> OnDragEvent
        {
            get
            {
                if (!this.TryGetComponent<BaseDragBehaviour>(out var cat)) return null;
                return cat.OnDragEvent;
            }
            set
            {
                var cat = this.GetOrAddComponent<BaseDragBehaviour>();
                cat.OnDragEvent = value;
            }
        }
        [Setting]
        public UnityEvent<PointerEventData> OnDropEvent
        {
            get
            {
                if (!this.TryGetComponent<BaseDropBehaviour>(out var cat)) return null;
                return cat.OnDropEvent;
            }
            set
            {
                var cat = this.GetOrAddComponent<BaseDropBehaviour>();
                cat.OnDropEvent = value;
            }
        }
        [Setting]
        public UnityEvent<PointerEventData> OnEndDragEvent
        {
            get
            {
                if (!this.TryGetComponent<BaseEndDragBehaviour>(out var cat)) return null;
                return cat.OnEndDragEvent;
            }
            set
            {
                var cat = this.GetOrAddComponent<BaseEndDragBehaviour>();
                cat.OnEndDragEvent = value;
            }
        }
        [Setting]
        public UnityEvent<PointerEventData> OnInitializePotentialDragEvent
        {
            get
            {
                if (!this.TryGetComponent<BaseInitializePotentialDragBehaviour>(out var cat))
                    return null;
                return cat.OnInitializePotentialDragEvent;
            }
            set
            {
                var cat = this.GetOrAddComponent<BaseInitializePotentialDragBehaviour>();
                cat.OnInitializePotentialDragEvent = value;
            }
        }
        [Setting]
        public UnityEvent<PointerEventData> OnPointerClickEvent
        {
            get
            {
                if (!this.TryGetComponent<BasePointerClickBehaviour>(out var cat))
                    return null;
                return cat.OnPointerClickEvent;
            }
            set
            {
                var cat = this.GetOrAddComponent<BasePointerClickBehaviour>();
                cat.OnPointerClickEvent = value;
            }
        }
        [Setting]
        public UnityEvent<PointerEventData> OnPointerDownEvent
        {
            get
            {
                if (!this.TryGetComponent<BasePointerDownBehaviour>(out var cat))
                    return null;
                return cat.OnPointerDownEvent;
            }
            set
            {
                var cat = this.GetOrAddComponent<BasePointerDownBehaviour>();
                cat.OnPointerDownEvent = value;
            }
        }
        [Setting]
        public UnityEvent<PointerEventData> OnPointerEnterEvent
        {
            get
            {
                if (!this.TryGetComponent<BasePointerEnterBehaviour>(out var cat))
                    return null;
                return cat.OnPointerEnterEvent;
            }
            set
            {
                var cat = this.GetOrAddComponent<BasePointerEnterBehaviour>();
                cat.OnPointerEnterEvent = value;
            }
        }
        [Setting]
        public UnityEvent<PointerEventData> OnPointerExitEvent
        {
            get
            {
                if (!this.TryGetComponent<BasePointerExitBehaviour>(out var cat))
                    return null;
                return cat.OnPointerExitEvent;
            }
            set
            {
                var cat = this.GetOrAddComponent<BasePointerExitBehaviour>();
                cat.OnPointerExitEvent = value;
            }
        }
        [Setting]
        public UnityEvent<PointerEventData> OnPointerUpEvent
        {
            get
            {
                if (!this.TryGetComponent<BasePointerUpBehaviour>(out var cat))
                    return null;
                return cat.OnPointerUpEvent;
            }
            set
            {
                var cat = this.GetOrAddComponent<BasePointerUpBehaviour>();
                cat.OnPointerUpEvent = value;
            }
        }
        [Setting]
        public UnityEvent<PointerEventData> OnScrollEvent
        {
            get
            {
                if (!this.TryGetComponent<BaseScrollBehaviour>(out var cat))
                    return null;
                return cat.OnScrollEvent;
            }
            set
            {
                var cat = this.GetOrAddComponent<BaseScrollBehaviour>();
                cat.OnScrollEvent = value;
            }
        }
        [Setting]
        public UnityEvent<BaseEventData> OnCancelEvent
        {
            get
            {
                if (!this.TryGetComponent<BaseCancelBehaviour>(out var cat))
                    return null;
                return cat.OnCancelEvent;
            }
            set
            {
                var cat = this.GetOrAddComponent<BaseCancelBehaviour>();
                cat.OnCancelEvent = value;
            }
        }
        [Setting]
        public UnityEvent<BaseEventData> OnDeselectEvent
        {
            get
            {
                if (!this.TryGetComponent<BaseDeselectBehaviour>(out var cat))
                    return null;
                return cat.OnDeselectEvent;
            }
            set
            {
                var cat = this.GetOrAddComponent<BaseDeselectBehaviour>();
                cat.OnDeselectEvent = value;
            }
        }
        [Setting]
        public UnityEvent<BaseEventData> OnSelectEvent
        {
            get
            {
                if (!this.TryGetComponent<BaseSelectBehaviour>(out var cat))
                    return null;
                return cat.OnSelectEvent;
            }
            set
            {
                var cat = this.GetOrAddComponent<BaseSelectBehaviour>();
                cat.OnSelectEvent = value;
            }
        }
        [Setting]
        public UnityEvent<BaseEventData> OnSubmitEvent
        {
            get
            {
                if (!this.TryGetComponent<BaseSubmitBehaviour>(out var cat))
                    return null;
                return cat.OnSubmitEvent;
            }
            set
            {
                var cat = this.GetOrAddComponent<BaseSubmitBehaviour>();
                cat.OnSubmitEvent = value;
            }
        }
        [Setting]
        public UnityEvent<BaseEventData> OnUpdateSelectedEvent
        {
            get
            {
                if (!this.TryGetComponent<BaseUpdateSelectedBehaviour>(out var cat))
                    return null;
                return cat.OnUpdateSelectedEvent;
            }
            set
            {
                var cat = this.GetOrAddComponent<BaseUpdateSelectedBehaviour>();
                cat.OnUpdateSelectedEvent = value;
            }
        }
        [Setting]
        public UnityEvent<AxisEventData> OnMoveEvent
        {
            get
            {
                if (!this.TryGetComponent<BaseMoveBehaviour>(out var cat))
                    return null;
                return cat.OnMoveEvent;
            }
            set
            {
                var cat = this.GetOrAddComponent<BaseMoveBehaviour>();
                cat.OnMoveEvent = value;
            }
        }

        public delegate bool HowSetupRaycastLocationValid(Vector2 sp, Camera eventCamera);
        public HowSetupRaycastLocationValid locationValid;

        public bool IsRaycastLocationValid(Vector2 sp, Camera eventCamera)
        {
            return locationValid?.Invoke(sp, eventCamera) ?? true;
        }

        private void Awake()
        {
            foreach (var item in GetComponents<IBehaviourOperator>())
            {
                Destroy(item as MonoBehaviour);
            }
        }
        private void OnDestroy()
        {
            foreach (var item in GetComponents<IBehaviourOperator>())
            {
                Destroy(item as MonoBehaviour);
            }
        }
    }
}

