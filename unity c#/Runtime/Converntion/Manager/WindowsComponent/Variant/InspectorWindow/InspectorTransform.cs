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
        [Resources] public ModernUIInputField ThisID;
        [Resources] public ModernUIInputField ParentID;
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
            return $"{vec.x:F4},{vec.y:F4},{vec.z:F4}";
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
            if (int.TryParse(str, out var code))
            {
                    var TargetTransform = (Transform)targetItem.GetValue();
                if (code == 0)
                {
                    TargetTransform.parent = null;
                    if (targetItem.target is IInspectorUpdater updater)
                    {
                        updater.OnInspectorUpdate();
                    }
                }
                else if (HierarchyWindow.instance.ContainsReference(code))
                {
                    var reference = HierarchyWindow.instance.GetReference(code);
                    if (reference is Component component)
                    {
                        TargetTransform.parent = component.transform;
                        if (targetItem.target is IInspectorUpdater updater)
                        {
                            updater.OnInspectorUpdate();
                        }
                    }
                    else if(reference is GameObject go)
                    {
                        TargetTransform.parent = go.transform;
                        if (targetItem.target is IInspectorUpdater updater)
                        {
                            updater.OnInspectorUpdate();
                        }
                    }
                }
                else
                {

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

            ThisID.InputFieldSource.Source.onEndEdit.AddListener(x => isEditing = false);
            ThisID.InputFieldSource.Source.onSelect.AddListener(x => isEditing = true);
            ThisID.InputFieldSource.Source.onSelect.AddListener(x => lastValue = ThisID.text);

            ParentID.AddListener(GenerateCallback_Transform);
            ParentID.InputFieldSource.Source.onEndEdit.AddListener(x => isEditing = false);
            ParentID.InputFieldSource.Source.onSelect.AddListener(x => isEditing = true);
            ParentID.InputFieldSource.Source.onSelect.AddListener(x => lastValue = ParentID.text);
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
            ThisID.text = targetItem.target.GetHashCode().ToString();
            if (TargetTransform.parent == null)
                ParentID.text = "0";
            else
                ParentID.text = TargetTransform.parent.GetHashCode().ToString();
        }

        private void FixedUpdate()
        {
            if (targetItem.UpdateType && !isEditing)
            {
                var TargetTransform = ((Transform)targetItem.GetValue());
                this.LocalPosition.text = ConvertString(TargetTransform.localPosition);
                this.Position.text = ConvertString(TargetTransform.position);
                this.Rotation.text = ConvertString(TargetTransform.eulerAngles);
                this.Scale.text = ConvertString(TargetTransform.localScale);
                this.ThisID.text = targetItem.target.GetHashCode().ToString();
                if (TargetTransform.parent == null)
                    ParentID.text = "0";
                else
                    ParentID.text = TargetTransform.parent.GetHashCode().ToString();
            }
        }

        private void Reset()
        {
            LocalPosition = transform.Find(nameof(LocalPosition)).GetComponent<ModernUIInputField>();
            Position = transform.Find(nameof(Position)).GetComponent<ModernUIInputField>();
            Rotation = transform.Find(nameof(Rotation)).GetComponent<ModernUIInputField>();
            Scale = transform.Find(nameof(Scale)).GetComponent<ModernUIInputField>();
            ThisID = transform.Find(nameof(ThisID)).GetComponent<ModernUIInputField>();
            ParentID = transform.Find(nameof(ParentID)).GetComponent<ModernUIInputField>();
        }
    }
}
