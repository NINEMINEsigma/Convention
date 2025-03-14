using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using static Convention.WindowsUI.Variant.PropertiesWindow;

namespace Convention.WindowsUI.Variant
{
    public class HierarchyItem : PropertyListItem
    {
        [Content, HopeNotNull] public object target;
        [Content] public bool IsEnableFocusWindow = true;

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
            InspectorWindow.instance.SetTarget(target, this);
            if (!IsEnableFocusWindow)
                return;
            FocusWindowIndictaor.instance.SetTargetRectTransform(TextRectTransform);
        }
    }
}
