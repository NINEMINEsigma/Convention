using System;
using System.Collections.Generic;
using Convention.WindowsUI;
using UnityEngine;

namespace Convention.Workflow
{
    [Serializable, ArgPackage]
    public class StepNodeInfo : NodeInfo
    {
        public string funcname = "unknown";
        protected override NodeInfo CreateTemplateNodeInfoBySelfType()
        {
            return new StepNodeInfo()
            {
                funcname = funcname
            };
        }
    }

    public class StepNode : Node
    {
        [Resources, OnlyNotNullMode] public ModernUIDropdown FunctionSelector;

        public StepNodeInfo MyStepInfo => info as StepNodeInfo;

        protected override void Start()
        {
            base.Start();
            FunctionSelector.AddListener(x =>
            {
                this.MyStepInfo.funcname = x;
                this.MyStepInfo.inmapping = new();
                foreach (var (name, type) in WorkflowManager.instance.GetFunctionModel(x).parameters)
                {
                    this.MyStepInfo.inmapping[name] = new NodeSlotInfo()
                    {
                        slotName = name,
                        typeIndicator = type,
                        IsInmappingSlot = true
                    };
                }
                this.MyStepInfo.outmapping = new();
                foreach (var (name, type) in WorkflowManager.instance.GetFunctionModel(x).returns)
                {
                    this.MyStepInfo.outmapping[name] = new NodeSlotInfo()
                    {
                        slotName = name,
                        typeIndicator = type,
                        IsInmappingSlot = false
                    };
                }
                this.title = x;
                this.FunctionSelector.gameObject.SetActive(false);
                this.ExtensionHeight = 10;
                this.ClearLink();
                this.ClearSlots();
                this.BuildSlots();
                this.BuildLink();
                this.InoutContainerPlane.rectTransform.SetInsetAndSizeFromParentEdge(RectTransform.Edge.Top, 50,
                    Mathf.Max(this.MyStepInfo.inmapping.Count, this.MyStepInfo.outmapping.Count) * SlotHeight + ExtensionHeight);
            });
        }

        private void OnEnable()
        {
            if (WorkflowManager.instance == null)
                return;
            FunctionSelector.ClearOptions();
            var names = WorkflowManager.instance.GetAllFunctionName();
            if (names.Count > 0)
            {
                foreach (var funcName in names)
                {
                    var funcModel = WorkflowManager.instance.GetFunctionModel(funcName);
                    if (funcModel.parameters.Count + funcModel.returns.Count != 0)
                        FunctionSelector.CreateOption(WorkflowManager.Transformer(funcName));
                }
            }
            else
            {
                FunctionSelector.CreateOption(WorkflowManager.Transformer("No Function Registered"));
            }
        }
    }
}
