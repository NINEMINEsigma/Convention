using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Convention.WindowsUI.Variant
{
    public class InspectorReference : InspectorDrawer
    {
        [Resources] public ModernUIInputField TextArea;
        public IAnyClass lastReference;
        [Content] public bool isEditing = false;

        private void OnCallback(string str)
        {
            if(str==null || str.Length==0)
            {
                targetItem.SetValue(null);
                if (targetItem.target is IInspectorUpdater updater)
                {
                    updater.OnInspectorUpdate();
                }
            }
            else if (int.TryParse(str, out var code) && HierarchyWindow.instance.ContainsReference(code))
            {
                targetItem.SetValue(HierarchyWindow.instance.GetReference(code));
                if (targetItem.target is IInspectorUpdater updater)
                {
                    updater.OnInspectorUpdate();
                }
            }
        }

        private void Start()
        {
            TextArea.AddListener(OnCallback);
            TextArea.InputFieldSource.Source.onEndEdit.AddListener(x => isEditing = false);
            TextArea.InputFieldSource.Source.onSelect.AddListener(x => isEditing = true);
        }

        private void OnEnable()
        {
            TextArea.interactable = targetItem.AbleChangeType;
            TextArea.text = targetItem.GetValue().GetHashCode().ToString();
        }

        private void FixedUpdate()
        {
            if (targetItem.UpdateType && !isEditing)
            {
                object value = targetItem.GetValue();
                if (value != null)
                {
                    TextArea.text = value.GetHashCode().ToString();
                }
                else
                {
                    TextArea.text = "";
                }
            }
        }

        private void Reset()
        {
            TextArea = GetComponent<ModernUIInputField>();
        }
    }
}
