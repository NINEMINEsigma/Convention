using System;
using System.Collections;
using System.Collections.Generic;
using Convention.WindowsUI.Variant;
using UnityEngine;

namespace Convention.WindowsUI.Variant
{
    public class ConsoleListItem : PropertyListItem
    {
        public string stackTrace;
        public bool IsEnableFocusWindow;


        [Content]
        public void OnFocusHierarchyItem()
        {
            if (!IsEnableFocusWindow)
                return;
            if (FocusWindowIndictaor.instance != null)
                FocusWindowIndictaor.instance.SetTargetRectTransform(TextRectTransform);
        }

    }
}
