using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Convention.WindowsUI.Variant
{
    public class InspectorText : InspectorDrawer
    {
        [Resources] public ModernUIInputField TextArea;

        private void OnCallback(string str)
        {
            targetItem.SetValue(str);
        }

        private void Start()
        {
            TextArea.AddListener(OnCallback);
        }

        public override void OnInspectorItemInit(InspectorItem item)
        {
            base.OnInspectorItemInit(item);
            TextArea.InputFieldSource.Source.interactable = item.AbleChangeType;
            TextArea.text = (string)targetItem.GetValue();
        }
    }
}
