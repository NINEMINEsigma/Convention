using System.Linq;
using Convention.WindowsUI;
using Convention.WindowsUI.Variant;
using UnityEngine;

namespace Convention.Workflow
{
    public class ValueNodeInfo : StartNodeInfo
    { 
        [InspectorDraw(InspectorDrawType.Text, name: "数值")]
        public float value = 0;
        [InspectorDraw(InspectorDrawType.Auto, name: "上限")]
        public float min = 100;
        [InspectorDraw(InspectorDrawType.Auto, name: "下限")]
        public float max = 0;

        public ValueNodeInfo() : this(0) { }
        public ValueNodeInfo(float value, string outmappingName = "value", int targetNodeID = -1, string targetSlotName = "value")
        {
            this.value = value;
            this.outmapping = new()
            {
                {
                    outmappingName, new NodeSlotInfo()
                    {
                        slotName = outmappingName,
                        typeIndicator = "string",
                        IsInmappingSlot = false,
                        targetNodeID = targetNodeID,
                        targetSlotName = targetSlotName,
                    }
                }
            };
        }

        protected override NodeInfo CreateTemplateNodeInfoBySelfType()
        {
            return new ValueNodeInfo()
            {
                value = value,
                min = min,
                max = max,
            };
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
