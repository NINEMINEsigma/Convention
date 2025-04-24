using System;
using Convention.WindowsUI;
using UnityEngine;

namespace Convention.Workflow
{
    [Serializable, ArgPackage]
    public class StepNodeInfo : NodeInfo
    {
        public string module = "global";
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
            var names = WorkflowManager.instance.GetAllModuleName();
            if (names.Count > 0)
            {
                foreach (var moduleName in names)
                {
                    this.FunctionSelector.CreateOption(WorkflowManager.Transformer(moduleName)).toggleEvents.AddListener(x =>
                    {
                        if (x)
                        {
                            this.FunctionSelector.ClearOptions();
                            foreach (var funcName in WorkflowManager.instance.GetAllFunctionName(moduleName))
                            {
                                var funcModel = WorkflowManager.instance.GetFunctionModel(moduleName, funcName);
                                if (funcModel.parameters.Count + funcModel.returns.Count != 0)
                                    this.FunctionSelector.CreateOption(WorkflowManager.Transformer(funcName)).toggleEvents.AddListener(y =>
                                    {
                                        if (y)
                                        {
                                            SetupWhenFunctionNameCatch(funcModel);
                                        }
                                    });
                            }
                        }
                    });
                }
            }
            else
            {
                FunctionSelector.CreateOption(WorkflowManager.Transformer("No Module Registered"));
            }
        }

        private void SetupWhenFunctionNameCatch(FunctionModel funcModel)
        {
            var oriExtensionHeight = this.ExtensionHeight;
            this.ExtensionHeight = 0;
            this.MyStepInfo.module = funcModel.module;
            this.MyStepInfo.funcname = funcModel.name;
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
            this.title = funcModel.name;
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
                SetupWhenFunctionNameCatch(WorkflowManager.instance.GetFunctionModel(MyStepInfo.module, MyStepInfo.funcname));
            }
        }
    }
}
