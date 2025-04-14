using System;
using System.Collections.Generic;
using System.Linq;
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

        private bool GetIsFlags()
        {
            if (enumType.IsEnum)
                return enumType.GetCustomAttributes(typeof(FlagsAttribute), true).Length != 0;
            else
                return false;
        }

        private string[] GetEnumNames()
        {
            if (enumType.IsEnum)
                return Enum.GetNames(enumType);
            else
            {
                var curValue = InspectorWindow.instance.GetTarget();
                var curType = curValue.GetType();
                var enumGeneratorField = curType.GetField(targetItem.targetDrawer.enumGenerater);
                if (enumGeneratorField != null)
                    return (enumGeneratorField.GetValue(curValue) as IEnumerable<string>).ToArray();
                else
                {
                    return (curType.GetMethod(targetItem.targetDrawer.enumGenerater).Invoke(curValue, new object[] { }) as IEnumerable<string>).ToArray();
                }
            }
        }

        private void OnEnable()
        {
            m_Dropdown.ClearOptions();
            enumType = targetItem.GetValue().GetType();
            isFlags = GetIsFlags();
            enumNames = GetEnumNames();
            if (enumType.IsEnum)
            {
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
            }
            else
            {
                string currentValue = (string)targetItem.GetValue();
                foreach (var name in enumNames)
                {
                    var item = m_Dropdown.CreateOption(name, T => targetItem.SetValue(name));
                    item.isOn = name == currentValue;
                }
            }
            m_Dropdown.interactable = targetItem.AbleChangeType;
            m_Dropdown.RefreshImmediate();
        }

        private void Reset()
        {
            m_Dropdown = GetComponent<ModernUIDropdown>();
        }

        private void Update()
        {
            if(targetItem.UpdateType)
            {
                foreach (var item in m_Dropdown.dropdownItems)
                {
                    if (isFlags)
                    {
                        item.isOn = ((int)Enum.Parse(enumType, item.itemName) & (int)targetItem.GetValue()) != 0;
                    }
                    else
                    {
                        if (enumType.IsEnum)
                            item.isOn = (int)Enum.Parse(enumType, item.itemName) == (int)targetItem.GetValue();
                        else
                            item.isOn = item.itemName == (string)targetItem.GetValue();
                    }
                }
            }
        }
    }
}
