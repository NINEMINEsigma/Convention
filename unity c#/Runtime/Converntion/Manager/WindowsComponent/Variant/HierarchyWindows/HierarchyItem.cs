using System.Collections.Generic;
using System;
using static Convention.WindowsUI.Variant.PropertiesWindow;
using System.Reflection;

namespace Convention.WindowsUI.Variant
{
    public interface IHierarchyItemTitle
    {
        string HierarchyItemTitle { get; }
    }

    public class HierarchyItem : PropertyListItem
    {
        [Content, HopeNotNull] public object m_target;
        public object target
        {
            get => m_target;
            set
            {
                m_target = value;
            }
        }
        [Content] public bool IsEnableFocusWindow = true;

        private void Update()
        {
            if (target is IHierarchyItemTitle ht)
            {
                this.title = ht.HierarchyItemTitle;
            }
        }

        protected override void Start()
        {
            base.Start();
            AddListener(OnFocusHierarchyItem);
        }
        private void OnDestroy()
        {
            if (InspectorWindow.instance.GetTarget() == target)
            {
                InspectorWindow.instance.ClearWindow();
            }
        }

        public List<ItemEntry> CreateSubPropertyItemWithBinders(params object[] binders)
        {
            List<ItemEntry> entries = CreateSubPropertyItem(Entry.rootWindow, binders.Length);
            for (int i = 0, e = binders.Length; i != e; i++)
            {
                var item = entries[i].ref_value.GetComponent<HierarchyItem>();
                item.target = binders[i];
                HierarchyWindow.instance.AddReference(binders[i], item);
            }
            return entries;
        }

        [Content]
        public void OnFocusHierarchyItem()
        {
            if (target == null)
            {
                throw new InvalidOperationException("target is null");
            }
            InspectorWindow.instance.SetTarget(target, this);
            if (!IsEnableFocusWindow)
                return;
            FocusWindowIndictaor.instance.SetTargetRectTransform(TextRectTransform);
        }
    }
}
