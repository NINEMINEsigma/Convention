using UnityEngine;
using UnityEngine.EventSystems;

namespace Convention.WindowsUI
{
    public class DragBehaviour : WindowsComponent
    {
        [SerializeField, Resources] private BehaviourContextManager Context = null;
        public BehaviourContextManager GetBehaviourContext()
        {
            if (Context == null)
                Context = this.GetOrAddComponent<BehaviourContextManager>();
            return Context;
        }
        public BehaviourContextManager DragBehaviourContext
        {
            get
            {
                if (Context == null)
                    Context = this.GetOrAddComponent<BehaviourContextManager>();
                return Context;
            }
        }

        [Setting] public bool isCanDrag = true;

        public void SetDragAble(bool isCanDrag)
        {
            this.isCanDrag = isCanDrag;
        }

        public void Init([In, Opt] RectTransform DragObjectInternal, [In, Opt] RectTransform DragAreaInternal)
        {
            if (DragObjectInternal != null)
                this.DragObjectInternal = DragObjectInternal;
            else if (this.DragObjectInternal == null)
                this.DragObjectInternal = rectTransform;
            if (DragAreaInternal != null)
                this.DragAreaInternal = DragAreaInternal;
            else if (this.DragAreaInternal == null)
                this.DragAreaInternal = rectTransform.parent as RectTransform;

            DragBehaviourContext.OnBeginDragEvent ??= new();
            DragBehaviourContext.OnDragEvent ??= new();

            DragBehaviourContext.OnBeginDragEvent.RemoveListener(this.OnBeginDrag);
            DragBehaviourContext.OnBeginDragEvent.AddListener(this.OnBeginDrag);
            DragBehaviourContext.OnDragEvent.RemoveListener(this.OnDrag);
            DragBehaviourContext.OnDragEvent.AddListener(this.OnDrag);
            DragBehaviourContext.locationValid = IsRaycastLocationValid;
        }

        [Setting] public bool IsAutoInit = true;
        private void Start()
        {
            Init(null, null);
        }

        private void Reset()
        {
            isCanDrag = true;
            DragObjectInternal = rectTransform;
        }


        [Setting] public bool topOnClick = true;

        [Content, Ignore, OnlyPlayMode, SerializeField] private Vector2 originalLocalPointerPosition;
        [Content, Ignore, OnlyPlayMode, SerializeField] private Vector3 originalPanelLocalPosition;

        [Resources, SerializeField] private RectTransform DragObjectInternal;

        [Resources, SerializeField, WhenAttribute.Not(nameof(DragObjectInternal), null)] private RectTransform DragAreaInternal;

        public void OnBeginDrag(PointerEventData data)
        {
            if (!isCanDrag) return;
            originalPanelLocalPosition = DragObjectInternal.localPosition;
            RectTransformUtility.ScreenPointToLocalPointInRectangle(DragAreaInternal, data.position, data.pressEventCamera, out originalLocalPointerPosition);
            gameObject.transform.SetAsLastSibling();

            if (topOnClick == true)
                DragObjectInternal.SetAsLastSibling();
        }

        public void OnDrag(PointerEventData data)
        {
            if (!isCanDrag) return;
            Vector2 localPointerPosition;
            if (RectTransformUtility.ScreenPointToLocalPointInRectangle(DragAreaInternal, data.position, data.pressEventCamera, out localPointerPosition))
            {
                Vector3 offsetToOriginal = localPointerPosition - originalLocalPointerPosition;
                DragObjectInternal.localPosition = originalPanelLocalPosition + offsetToOriginal;
            }

            ClampToArea();
        }

        private void ClampToArea()
        {
            Vector3 pos = DragObjectInternal.localPosition;

            Vector3 minPosition = DragAreaInternal.rect.min - DragObjectInternal.rect.min;
            Vector3 maxPosition = DragAreaInternal.rect.max - DragObjectInternal.rect.max;

            pos.x = Mathf.Clamp(DragObjectInternal.localPosition.x, minPosition.x, maxPosition.x);
            pos.y = Mathf.Clamp(DragObjectInternal.localPosition.y, minPosition.y, maxPosition.y);

            DragObjectInternal.localPosition = pos;
        }

        public bool IsRaycastLocationValid(Vector2 sp, Camera eventCamera)
        {
            return isCanDrag || transform.childCount != 0;
        }
    }
}
