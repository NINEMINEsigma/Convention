using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Convention.WindowsUI.Variant
{
    public class InspectorToggle : InspectorDrawer
    {
        [Resources] public ModernUIToggle Toggle;

        private void OnCallback(bool value)
        {
            targetItem.SetValue(value);
        }

        private void Start()
        {
            Toggle.AddListener(OnCallback);
        }

        private void Update()
        {
            if (targetItem.UpdateType)
            {
                Toggle.ref_value = (bool)targetItem.GetValue();
            }
        }

        public override void OnInspectorItemInit(InspectorItem item)
        {
            base.OnInspectorItemInit(item);
            Toggle.interactable = item.AbleChangeType;
            Toggle.ref_value = (bool)item.GetValue();
        }

        private void Reset()
        {
            Toggle = GetComponent<ModernUIToggle>();
        }
    }
}
