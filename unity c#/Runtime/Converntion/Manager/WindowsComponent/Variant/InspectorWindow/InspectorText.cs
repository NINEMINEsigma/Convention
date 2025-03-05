using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Convention.WindowsUI.Variant
{
    public class InspectorText : InspectorDrawer
    {
        [Resources] public ModernUIInputField TextArea;
        [Content] public bool isEditing = false;

        private void OnCallback(string str)
        {
            targetItem.SetValue(str);
        }

        private void Start()
        {
            TextArea.AddListener(OnCallback);
            TextArea.InputFieldSource.Source.onEndEdit.AddListener(x => isEditing = false);
            TextArea.InputFieldSource.Source.onSelect.AddListener(x => isEditing = true);
        }

        public override void OnInspectorItemInit(InspectorItem item)
        {
            base.OnInspectorItemInit(item);
            TextArea.interactable = item.AbleChangeType;
            TextArea.text = targetItem.GetValue().ToString();
        }

        private void Update()
        {
            if (targetItem.UpdateType && !isEditing)
            {
                TextArea.text = targetItem.GetValue().ToString();
            }
        }

        private void Reset()
        {
            TextArea = GetComponent<ModernUIInputField>();
        }
    }
}
