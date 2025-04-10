using System.Linq;
using Convention.WindowsUI;
using Convention.WindowsUI.Variant;
using UnityEngine;

namespace Convention.Workflow
{
    public class TextNodeInfo : StartNodeInfo
    {
        [InspectorDraw(InspectorDrawType.Text, name: "ÎÄ±¾ÄÚÈÝ")]
        public string text;

        public TextNodeInfo() : this("") { }
        public TextNodeInfo(string text, string outmappingName = "text", int targetNodeID = -1, string targetSlotName = "text")
        {
            this.text = text;
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
            return new TextNodeInfo();
        }
    }

    public class TextNode : StartNode,IText
    {
        [Resources, OnlyNotNullMode] public ModernUIInputField InputField;
        [Content, OnlyPlayMode] public bool isEditing = false;

        public TextNodeInfo MyTextNodeInfo => this.info as TextNodeInfo;

        public string text { get => ((IText)this.InputField).text; set => ((IText)this.InputField).text = value; }

        [Resources, OnlyNotNullMode] public NodeSlot OutputSlot;

        protected override void Start()
        {
            base.Start();
            InputField.InputFieldSource.Source.onSelect.AddListener(_=>isEditing = true);
            InputField.InputFieldSource.Source.onEndEdit.AddListener(str =>
            {
                MyTextNodeInfo.text = str;
                isEditing = false;
            });
        }

        protected override void WhenSetup(NodeInfo info)
        {
            var pair = info.outmapping.First();
            m_Outmapping.Clear();
            var slotInfo = pair.Value.TemplateClone();
            slotInfo.parentNode = this;
            OutputSlot.SetupFromInfo(slotInfo);
            m_Outmapping[pair.Key] = OutputSlot;
        }

        private void LateUpdate()
        {
            if (info != null && this.isEditing == false && RectTransformExtension.IsVisible(this.rectTransform))
            {
                this.text = this.MyTextNodeInfo.text;
            }
        }
    }
}
