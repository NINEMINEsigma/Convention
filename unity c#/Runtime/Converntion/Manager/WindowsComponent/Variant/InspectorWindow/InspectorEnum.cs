using System;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

namespace Convention.WindowsUI.Variant
{
    public class InspectorEnum : InspectorDrawer
    {
        [Resources] public ModernUIDropdown m_Dropdown;

        private Type enumType;
        private bool isFlags;
        private string[] enumNames;

        public override void OnInspectorItemInit(InspectorItem item)
        {
            base.OnInspectorItemInit(item);
        }

        private void OnEnable()
        {
            m_Dropdown.ClearOptions();
            enumType = targetItem.GetValue().GetType();
            isFlags = enumType.GetCustomAttributes(typeof(FlagsAttribute), true).Length != 0;
            enumNames = Enum.GetNames(enumType);
            int currentValue = (int)targetItem.GetValue();
            foreach (var name in enumNames)
            {
                var item = m_Dropdown.CreateOption(name, T =>
                {
                    if (Enum.TryParse(enumType, name, out var result))
                    {
                        if (isFlags)
                        {
                            targetItem.SetValue((int)targetItem.GetValue() | (int)result);
                        }
                        else if (T)
                        {
                            targetItem.SetValue(result);
                        }
                    }
                });
                if (isFlags)
                {
                    item.isOn = ((int)Enum.Parse(enumType, name) & currentValue) != 0;
                }
                else
                {
                    item.isOn = (int)Enum.Parse(enumType, name) == currentValue;
                }
            }
            m_Dropdown.interactable = targetItem.AbleChangeType;
        }

        private void Reset()
        {
            m_Dropdown = GetComponent<ModernUIDropdown>();
        }

        private void Update()
        {
            if(targetItem.UpdateType)
            {
                int currentValue = (int)targetItem.GetValue();
                foreach (var item in m_Dropdown.dropdownItems)
                {
                    if (isFlags)
                    {
                        item.isOn = ((int)Enum.Parse(enumType, item.itemName) & currentValue) != 0;
                    }
                    else
                    {
                        item.isOn = (int)Enum.Parse(enumType, item.itemName) == currentValue;
                    }
                }
            }
        }
    }
}
