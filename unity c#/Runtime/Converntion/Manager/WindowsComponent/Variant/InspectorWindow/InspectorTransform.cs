using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

namespace Convention.WindowsUI.Variant
{
    public class InspectorTransform : InspectorDrawer
    {
        [Resources] public ModernUIInputField LocalPosition;
        [Resources] public ModernUIInputField Position;
        [Resources] public ModernUIInputField Rotation;
        [Resources] public ModernUIInputField Scale;
        [Content] public bool isEditing = false;
        [Content] public Vector3 lastValue;

        private static bool Prase(string str, out Vector3 result)
        {
            var strs = str.Split(',');
            result = new();
            if (strs.Length != 3)
                return false;
            if (float.TryParse(strs[0], out float x) == false)
                return false;
            if (float.TryParse(strs[1], out float y) == false)
                return false;
            if (float.TryParse(strs[2], out float z) == false)
                return false;
            result.x = x;
            result.y = y;
            result.z = z;
            return true;
        }
        private static string ConvertString(Vector3 vec)
        {
            return $"{vec.x},{vec.y},{vec.z}";
        }

        private UnityAction<string> GenerateCallback(Action<Vector3> action)
        {
            void OnCallback(string str)
            {
                if(Prase(str,out var result))
                {
                    action(result);
                }
                else
                {
                    action(lastValue);
                }
            }
            return OnCallback;
        }

        private void Start()
        {
            LocalPosition.AddListener(GenerateCallback(x => ((Transform)targetItem.GetValue()).localPosition = x));
            LocalPosition.InputFieldSource.Source.onEndEdit.AddListener(x => isEditing = false);
            LocalPosition.InputFieldSource.Source.onSelect.AddListener(x => isEditing = true);
            LocalPosition.InputFieldSource.Source.onSelect.AddListener(x => lastValue = ((Transform)targetItem.GetValue()).localPosition);
            Position.AddListener(GenerateCallback(x => ((Transform)targetItem.GetValue()).position = x));
            Position.InputFieldSource.Source.onEndEdit.AddListener(x => isEditing = false);
            Position.InputFieldSource.Source.onSelect.AddListener(x => isEditing = true);
            Position.InputFieldSource.Source.onSelect.AddListener(x => lastValue = ((Transform)targetItem.GetValue()).position);

            Rotation.AddListener(GenerateCallback(x => ((Transform)targetItem.GetValue()).eulerAngles = x));
            Rotation.InputFieldSource.Source.onEndEdit.AddListener(x => isEditing = false);
            Rotation.InputFieldSource.Source.onSelect.AddListener(x => isEditing = true);
            Rotation.InputFieldSource.Source.onSelect.AddListener(x => lastValue = ((Transform)targetItem.GetValue()).eulerAngles);

            Scale.AddListener(GenerateCallback(x => ((Transform)targetItem.GetValue()).localScale = x));
            Scale.InputFieldSource.Source.onEndEdit.AddListener(x => isEditing = false);
            Scale.InputFieldSource.Source.onSelect.AddListener(x => isEditing = true);
            Scale.InputFieldSource.Source.onSelect.AddListener(x => lastValue = ((Transform)targetItem.GetValue()).localScale);
        }

        public override void OnInspectorItemInit(InspectorItem item)
        {
            base.OnInspectorItemInit(item);
            LocalPosition.interactable = item.AbleChangeType;
            LocalPosition.text = ConvertString(((Transform)targetItem.GetValue()).localPosition);
            Position.interactable = item.AbleChangeType;
            Position.text = ConvertString(((Transform)targetItem.GetValue()).position);
            Rotation.interactable = item.AbleChangeType;
            Rotation.text = ConvertString(((Transform)targetItem.GetValue()).eulerAngles);
            Scale.interactable = item.AbleChangeType;
            Scale.text = ConvertString(((Transform)targetItem.GetValue()).localScale);
        }

        private void Update()
        {
            if (targetItem.UpdateType && !isEditing)
            {
                LocalPosition.text = ConvertString(((Transform)targetItem.GetValue()).position);
                Position.text = ConvertString(((Transform)targetItem.GetValue()).position);
                Rotation.text = ConvertString(((Transform)targetItem.GetValue()).eulerAngles);
                Scale.text = ConvertString(((Transform)targetItem.GetValue()).localScale);
            }
        }

        private void Reset()
        {
            LocalPosition = transform.Find(nameof(LocalPosition)).GetComponent<ModernUIInputField>();
            Position = transform.Find(nameof(Position)).GetComponent<ModernUIInputField>();
            Rotation = transform.Find(nameof(Rotation)).GetComponent<ModernUIInputField>();
            Scale = transform.Find(nameof(Scale)).GetComponent<ModernUIInputField>();
        }
    }
}
