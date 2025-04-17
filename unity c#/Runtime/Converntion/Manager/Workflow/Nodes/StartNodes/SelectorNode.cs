using System;
using System.Collections;
using System.Collections.Generic;
using Convention.WindowsUI;
using Convention.WindowsUI.Variant;
using UnityEngine;

namespace Convention.Workflow
{
    public class SelectorNodeInfo : StartNodeInfo
    {
        public virtual IEnumerable<string> EnumNamesGenerater()
        {
            return new List<string>()
            {
                 WorkflowManager.Transformer("unknown")
            };
        }

        [NonSerialized] private string l_select = WorkflowManager.Transformer(nameof(select));
        [InspectorDraw(InspectorDrawType.Enum, true, false, nameGenerater: nameof(l_select), enumGenerater: nameof(EnumNamesGenerater))]
        public string select = "unknown";

        public SelectorNodeInfo() : this(WorkflowManager.Transformer(nameof(SelectorNode))) { }
        public SelectorNodeInfo(string select, string outputName = "select")
        {
            this.select = select;
            this.outmapping = new()
            {
                {
                    outputName, new NodeSlotInfo()
                    {
                        slotName = outputName,
                        typeIndicator="string",
                        IsInmappingSlot=false
                    }
                }
            };
            this.inmapping = new();
            this.title = "Selector";
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

        protected virtual void RebuildDropDown(ModernUIDropdown dropdown, SelectorNodeInfo info)
        {
            try
            {
                foreach (var llm in info.EnumNamesGenerater())
                {
                    dropdown.CreateOption(llm);
                }
            }
            catch (Exception)
            {
                GameObject.Destroy(this.gameObject);
            }
        }

        protected override void WhenSetup(NodeInfo info)
        {
            RebuildDropDown(DropDown, info as SelectorNodeInfo);
            base.WhenSetup(info);
        }
    }
}
