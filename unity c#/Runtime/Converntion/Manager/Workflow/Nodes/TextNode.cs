using System.Collections;
using System.Collections.Generic;
using Convention.WindowsUI;
using UnityEngine;

namespace Convention.Workflow
{
    public class TextNodeInfo : StartNodeInfo
    {
        public string text;

        public TextNodeInfo() { }
        public TextNodeInfo(string text, string outmappingName="text",int targetNodeID=-1,string targetSlotName="text")
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
    }

    public class TextNode : StartNode
    {
        [Resources, OnlyNotNullMode] public ModernUIInputField InputField;
        [Content, OnlyPlayMode] public bool isEditing = false;

        public TextNodeInfo MyTextNodeInfo => this.info as TextNodeInfo;

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

        private void LateUpdate()
        {
            if (this.isEditing == false)
            {

            }
        }
    }
}
