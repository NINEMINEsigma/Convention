using System.Linq;
using Convention.WindowsUI;
using Convention.WindowsUI.Variant;
using UnityEngine;

namespace Convention.Workflow
{
    public class TextNodeInfo : StartNodeInfo
    {
        private string l_text => WorkflowManager.Transformer(nameof(text));
        [InspectorDraw(InspectorDrawType.Text, nameGenerater: nameof(l_text))]
        public string text;

        public TextNodeInfo() : this(WorkflowManager.Transformer("text")) { }
        public TextNodeInfo(string text, string outmappingName = "text")
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
                    }
                }
            };
            this.inmapping = new();
            this.title = "Text";
        }

        protected override NodeInfo CreateTemplateNodeInfoBySelfType()
        {
            return new TextNodeInfo()
            {
                text = text
            };
        }
    }

    public class TextNode : StartNode, IText
    {
        [Resources, OnlyNotNullMode] public ModernUIInputField InputField;
        [Content, OnlyPlayMode] public bool isEditing = false;

        public TextNodeInfo MyTextNodeInfo => this.info as TextNodeInfo;

        public string text { get => ((IText)this.InputField).text; set => ((IText)this.InputField).text = value; }


        protected override void Start()
        {
            base.Start();
            InputField.InputFieldSource.Source.onSelect.AddListener(_ => isEditing = true);
            InputField.InputFieldSource.Source.onEndEdit.AddListener(str =>
            {
                MyTextNodeInfo.text = str;
                isEditing = false;
            });
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
