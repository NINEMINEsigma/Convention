using System;
using System.Collections.Generic;
using Convention.WindowsUI;
using UnityEngine;

namespace Convention.Workflow
{
    [Serializable, ArgPackage]
    public class StepNodeInfo : NodeInfo
    {
        public string funcname = "";
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
                        this.FunctionSelector.CreateOption(WorkflowManager.Transformer(funcName)).toggleEvents.AddListener(x =>
                        {
                            if (x)
                            {
                                SetupWhenFunctionNameCatch(funcModel);
                            }
                        });
                }
            }
            else
            {
                FunctionSelector.CreateOption(WorkflowManager.Transformer("No Function Registered"));
            }
        }

        private void SetupWhenFunctionNameCatch(FunctionModel funcModel)
        {
            string funcName = funcModel.name;
            var oriExtensionHeight = this.ExtensionHeight;
            this.ExtensionHeight = 0;
            this.MyStepInfo.funcname = funcName;
            this.MyStepInfo.inmapping = new();
            foreach (var (name, type) in funcModel.parameters)
            {
                this.MyStepInfo.inmapping[name] = new NodeSlotInfo()
                {
                    slotName = name,
                    typeIndicator = type,
                    IsInmappingSlot = true
                };
            }
            this.MyStepInfo.outmapping = new();
            foreach (var (name, type) in funcModel.returns)
            {
                this.MyStepInfo.outmapping[name] = new NodeSlotInfo()
                {
                    slotName = name,
                    typeIndicator = type,
                    IsInmappingSlot = false
                };
            }
            this.title = funcName;
            this.FunctionSelector.gameObject.SetActive(false);
            this.ExtensionHeight = 10;
            this.ClearLink();
            this.ClearSlots();
            this.BuildSlots();
            this.BuildLink();
            this.InoutContainerPlane.rectTransform.sizeDelta = new Vector2(
                this.InoutContainerPlane.rectTransform.sizeDelta.x,
                this.InoutContainerPlane.rectTransform.sizeDelta.y + oriExtensionHeight
                );
        }

        protected override void WhenSetup(NodeInfo info)
        {
            base.WhenSetup(info);
            if (string.IsNullOrEmpty(MyStepInfo.funcname) == false)
            {
                SetupWhenFunctionNameCatch(WorkflowManager.instance.GetFunctionModel(MyStepInfo.funcname));
            }
        }
    }
}
