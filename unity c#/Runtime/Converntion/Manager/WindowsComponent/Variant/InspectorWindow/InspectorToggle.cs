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
            if (targetItem.target is IInspectorUpdater updater)
            {
                updater.OnInspectorUpdate();
            }
        }

        private void Start()
        {
            Toggle.AddListener(OnCallback);
        }

        private void FixedUpdate()
        {
            if (targetItem.UpdateType)
            {
                Toggle.ref_value = (bool)targetItem.GetValue();
            }
        }

        private void OnEnable()
        {
            Toggle.interactable = targetItem.AbleChangeType;
            Toggle.ref_value = (bool)targetItem.GetValue();
        }

        private void Reset()
        {
            Toggle = GetComponent<ModernUIToggle>();
        }
    }
}
