using System;
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
            if (targetItem.target is IInspectorUpdater updater)
            {
                updater.OnInspectorUpdate();
            }
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
        }

        private void OnEnable()
        {
            TextArea.interactable = targetItem.AbleChangeType;
            try
            {
                TextArea.interactable = ConventionUtility.IsString(targetItem.GetValue().GetType());
            }
            catch (Exception) { }
            TextArea.text = targetItem.GetValue().ToString();
        }

        private void FixedUpdate()
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
