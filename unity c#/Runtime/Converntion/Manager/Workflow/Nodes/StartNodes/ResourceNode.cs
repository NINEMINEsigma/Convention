using System;
using System.Collections;
using System.Collections.Generic;
using Convention.WindowsUI;
using Convention.WindowsUI.Variant;
using UnityEngine;

namespace Convention.Workflow
{
    [Serializable, ArgPackage]
    public class ResourceNodeInfo : StartNodeInfo
    {
        [NonSerialized] private string l_resource = WorkflowManager.Transformer(nameof(resource));
        [InspectorDraw(InspectorDrawType.Text, true, true, nameof(l_resource))]
        public string resource = "unknown";

        public ResourceNodeInfo() : this("") { }
        public ResourceNodeInfo(string resource, string outmappingName = "value")
        {
            this.resource = resource;
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
            this.title = "Resource";
        }
        protected override NodeInfo CreateTemplateNodeInfoBySelfType()
        {
            return new ResourceNodeInfo()
            {
                resource = resource
            };
        }
    }

    public class ResourceNode : StartNode, IText
    {
        [Resources, OnlyNotNullMode] public ModernUIInputField InputField;
        [Content, OnlyPlayMode] public bool isEditing = false;

        public ResourceNodeInfo MyResourceNodeInfo => this.info as ResourceNodeInfo;

        public string text { get => ((IText)this.InputField).text; set => ((IText)this.InputField).text = value; }


        protected override void Start()
        {
            base.Start();
            InputField.InputFieldSource.Source.onSelect.AddListener(_ => isEditing = true);
            InputField.InputFieldSource.Source.onEndEdit.AddListener(str =>
            {
                MyResourceNodeInfo.resource = str;
                isEditing = false;
            });
        }

        private void LateUpdate()
        {
            if (info != null && this.isEditing == false && RectTransformExtension.IsVisible(this.rectTransform))
            {
                this.text = this.MyResourceNodeInfo.resource;
            }
        }
    }
}
