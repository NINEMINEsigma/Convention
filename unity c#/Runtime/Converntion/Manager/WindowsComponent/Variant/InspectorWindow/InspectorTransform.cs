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
        [Resources] public ModernUIInputField Parent;
        [Content] public bool isEditing = false;
        [Content] public string lastValue;

        private static bool Parse(string str, out Vector3 result)
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
                if(Parse(str,out var result))
                {
                    action(result);
                    if (targetItem.target is IInspectorUpdater updater)
                    {
                        updater.OnInspectorUpdate();
                    }
                }
                else
                {
                    if (Parse(lastValue, out var lastVec))
                        action(lastVec);
                    else
                        throw new InvalidOperationException();
                }
            }
            return OnCallback;
        }

        private void GenerateCallback_Transform(string str)
        {
            if (int.TryParse(str, out var code) && HierarchyWindow.instance.ContainsReference(code))
            {
                var reference = HierarchyWindow.instance.GetReference(code);
                if (reference is Component component)
                {
                    var TargetTransform = (Transform)targetItem.GetValue();
                    TargetTransform.parent = component.transform;
                    if (targetItem.target is IInspectorUpdater updater)
                    {
                        updater.OnInspectorUpdate();
                    }
                }
            }
        }

        private void Start()
        {
            var TargetTransform = (Transform)targetItem.GetValue();
            LocalPosition.AddListener(GenerateCallback(x => TargetTransform.localPosition = x));
            LocalPosition.InputFieldSource.Source.onEndEdit.AddListener(x => isEditing = false);
            LocalPosition.InputFieldSource.Source.onSelect.AddListener(x => isEditing = true);
            LocalPosition.InputFieldSource.Source.onSelect.AddListener(x => lastValue = ConvertString(TargetTransform.localPosition));

            Position.AddListener(GenerateCallback(x => TargetTransform.position = x));
            Position.InputFieldSource.Source.onEndEdit.AddListener(x => isEditing = false);
            Position.InputFieldSource.Source.onSelect.AddListener(x => isEditing = true);
            Position.InputFieldSource.Source.onSelect.AddListener(x => lastValue = ConvertString(TargetTransform.position));

            Rotation.AddListener(GenerateCallback(x => TargetTransform.eulerAngles = x));
            Rotation.InputFieldSource.Source.onEndEdit.AddListener(x => isEditing = false);
            Rotation.InputFieldSource.Source.onSelect.AddListener(x => isEditing = true);
            Rotation.InputFieldSource.Source.onSelect.AddListener(x => lastValue = ConvertString(TargetTransform.eulerAngles));

            Scale.AddListener(GenerateCallback(x => TargetTransform.localScale = x));
            Scale.InputFieldSource.Source.onEndEdit.AddListener(x => isEditing = false);
            Scale.InputFieldSource.Source.onSelect.AddListener(x => isEditing = true);
            Scale.InputFieldSource.Source.onSelect.AddListener(x => lastValue = ConvertString(TargetTransform.localScale));

            Parent.AddListener(GenerateCallback_Transform);
            Parent.InputFieldSource.Source.onEndEdit.AddListener(x => isEditing = false);
            Parent.InputFieldSource.Source.onSelect.AddListener(x => isEditing = true);
            Parent.InputFieldSource.Source.onSelect.AddListener(x => lastValue = targetItem.target.GetHashCode().ToString());
        }

        private void OnEnable()
        {
            LocalPosition.interactable = targetItem.AbleChangeType;
            var TargetTransform = ((Transform)targetItem.GetValue());
            this.LocalPosition.text = ConvertString(TargetTransform.localPosition);
            Position.interactable = targetItem.AbleChangeType;
            this.Position.text = ConvertString(TargetTransform.position);
            Rotation.interactable = targetItem.AbleChangeType;
            this.Rotation.text = ConvertString(TargetTransform.eulerAngles);
            Scale.interactable = targetItem.AbleChangeType;
            this.Scale.text = ConvertString(TargetTransform.localScale);
            Parent.interactable = targetItem.AbleChangeType;
            Parent.text = targetItem.target.GetHashCode().ToString();
        }

        private void FixedUpdate()
        {
            if (targetItem.UpdateType && !isEditing)
            {
                var TargetTransform = ((Transform)targetItem.GetValue());
                this.LocalPosition.text = ConvertString(TargetTransform.position);
                this.Position.text = ConvertString(TargetTransform.position);
                this.Rotation.text = ConvertString(TargetTransform.eulerAngles);
                this.Scale.text = ConvertString(TargetTransform.localScale);
                this.Parent.text = targetItem.target.GetHashCode().ToString();
            }
        }

        private void Reset()
        {
            LocalPosition = transform.Find(nameof(LocalPosition)).GetComponent<ModernUIInputField>();
            Position = transform.Find(nameof(Position)).GetComponent<ModernUIInputField>();
            Rotation = transform.Find(nameof(Rotation)).GetComponent<ModernUIInputField>();
            Scale = transform.Find(nameof(Scale)).GetComponent<ModernUIInputField>();
            Parent = transform.Find(nameof(Parent)).GetComponent<ModernUIInputField>();
        }
    }
}
