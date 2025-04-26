using System;
using System.Linq;
using Convention.WindowsUI;
using Convention.WindowsUI.Variant;
using UnityEngine;

namespace Convention.Workflow
{
    public class ValueNodeInfo : StartNodeInfo
    {
        [NonSerialized] private string l_value = WorkflowManager.Transformer(nameof(value));
        [InspectorDraw(InspectorDrawType.Text, true, true, nameGenerater: nameof(l_value))]
        public float value = 0;
        [NonSerialized] private string l_min = WorkflowManager.Transformer(nameof(min));
        [InspectorDraw(InspectorDrawType.Auto, true, true, nameGenerater: nameof(l_min))]
        public float min = 0;
        [NonSerialized]private string l_max = WorkflowManager.Transformer(nameof(max));
        [InspectorDraw(InspectorDrawType.Auto, true, true, nameGenerater: nameof(l_max))]
        public float max = 1;

        public ValueNodeInfo() : this(0) { }
        public ValueNodeInfo(float value, string outmappingName = "value")
        {
            this.value = value;
            this.outmapping = new()
            {
                {
                    outmappingName, new NodeSlotInfo()
                    {
                        slotName = outmappingName,
                        typeIndicator = "float",
                        IsInmappingSlot = false,
                    }
                }
            };
            this.inmapping = new();
            this.title = "Value";
        }

        protected override NodeInfo CreateTemplate()
        {
            return new ValueNodeInfo();
        }
        protected override void CloneValues([In] NodeInfo clonen)
        {
            var info = ((ValueNodeInfo)clonen);
            info.value = value;
            info.min = min;
            info.max = max;
            base.CloneValues(clonen);
        }
    }

    public class ValueNode : StartNode, IText
    {
        [Resources, OnlyNotNullMode] public ModernUIInputField InputField;
        [Resources, OnlyNotNullMode] public ModernUIFillBar RangeBar;
        [Content, OnlyPlayMode] public bool isEditing = false;

        public ValueNodeInfo MyValueNodeInfo => this.info as ValueNodeInfo;

        public string text { get => ((IText)this.InputField).text; set => ((IText)this.InputField).text = value; }

        protected override void Start()
        {
            base.Start();
            InputField.InputFieldSource.Source.onSelect.AddListener(_ => isEditing = true);
            InputField.InputFieldSource.Source.onEndEdit.AddListener(str =>
            {
                if (float.TryParse(str, out float value))
                    MyValueNodeInfo.value = value;
                else
                    MyValueNodeInfo.value = 0;
                isEditing = false;
            });
        }

        protected override void WhenSetup(NodeInfo info)
        {
            base.WhenSetup(info);
            RangeBar.minValue = MyValueNodeInfo.min;
            RangeBar.maxValue = MyValueNodeInfo.max;
            RangeBar.SetValue(MyValueNodeInfo.value);
        }

        private void LateUpdate()
        {
            if (info != null && this.isEditing == false && RectTransformExtension.IsVisible(this.rectTransform))
            {
                RangeBar.minValue = MyValueNodeInfo.min;
                RangeBar.maxValue = MyValueNodeInfo.max;
                RangeBar.SetValue(MyValueNodeInfo.value);
            }
        }
    }
}
