using System.Collections;
using System.Collections.Generic;
using Convention.WindowsUI;
using Convention.WindowsUI.Variant;
using UnityEngine;

namespace Convention.Workflow
{
    public class SelectorNodeInfo : StartNodeInfo
    {
        [InspectorDraw(InspectorDrawType.Text, true, false, "选择")]
        public string select = "unknown";

        public SelectorNodeInfo() : this(WorkflowManager.Transformer(nameof(SelectorNode))) { }
        public SelectorNodeInfo(string select, string outputName = "select", string title = "选择器")
        {
            this.select = select;
            this.outmapping = new()
            {
                {
                    outputName, new NodeSlotInfo()
                    {
                        slotName=outputName,
                        typeIndicator="string",
                        IsInmappingSlot=false
                    }
                }
            };
            this.inmapping = new();
            this.title = title;
        }

        protected override NodeInfo CreateTemplateNodeInfoBySelfType()
        {
            return new SelectorNodeInfo()
            {
                select = select,
                outmapping = outmapping
            };
        }
    }

    public class SelectorNode : StartNode
    {
        [Resources, OnlyNotNullMode,SerializeField] private ModernUIDropdown DropDown;

        public SelectorNodeInfo MySelectorInfo => info as SelectorNodeInfo;

        protected override void Start()
        {
            base.Start();
            DropDown.AddListener(x => MySelectorInfo.select = x);
        }

        protected virtual void RebuildDropDown(ModernUIDropdown dropdown)
        {

        }

        protected override void WhenSetup(NodeInfo info)
        {
            RebuildDropDown(DropDown);
            base.WhenSetup(info);
        }
    }
}
