using System;
using System.Collections.Generic;
using Convention.WindowsUI;
using UnityEngine;

namespace Convention.Workflow
{
    [Serializable, ArgPackage]
    public class StepNodeInfo : NodeInfo
    {
        public string module = "global";
        public string funcname = "";
        protected override NodeInfo CreateTemplate()
        {
            return new StepNodeInfo();
        }
        protected override void CloneValues([In] NodeInfo clonen)
        {
            var info = (StepNodeInfo)clonen;
            info.module = module;
            info.funcname = funcname;
            base.CloneValues(clonen);
        }
    }

    public class StepNode : Node
    {
        [Resources, OnlyNotNullMode] public ModernUIDropdown FunctionSelector;

        public StepNodeInfo MyStepInfo => info as StepNodeInfo;

        private void ClearSelector()
        {
            FunctionSelector.ClearOptions();
        }

        private void OnEnable()
        {
            if (WorkflowManager.instance == null)
                return;
            ClearSelector();
            var names = WorkflowManager.instance.GetAllModuleName();
            if (names.Count > 0)
            {
                SelectModel(names);
            }
            else
            {
                FunctionSelector.CreateOption(WorkflowManager.Transformer("No Module Registered"));
            }
        }

        private void SelectModel(List<string> names)
        {
            foreach (var moduleName in names)
            {
                this.FunctionSelector.CreateOption(WorkflowManager.Transformer(moduleName)).toggleEvents.AddListener(x =>
                {
                    if (x)
                    {
                        ClearSelector();
                        SelectFunctionModel(moduleName);
                    }
                });
            }
            this.FunctionSelector.RefreshImmediate();
        }

        private void SelectFunctionModel(string moduleName)
        {
            foreach (var funcModel in WorkflowManager.instance.GetAllFunctionModel(moduleName))
            {
                this.FunctionSelector.CreateOption(WorkflowManager.Transformer(funcModel.name)).toggleEvents.AddListener(y =>
                {
                    if (y)
                    {
                        SetupWhenFunctionNameCatch(funcModel);
                    }
                });
            }
            this.FunctionSelector.RefreshImmediate();
        }

        public void SetupWhenFunctionNameCatch(FunctionModel funcModel)
        {
            var oriExtensionHeight = this.ExtensionHeight;
            this.ExtensionHeight = 0;
            this.MyStepInfo.module = funcModel.module;
            this.MyStepInfo.funcname = funcModel.name;
            this.MyStepInfo.inmapping = new();
            if (this.MyStepInfo.title == this.MyStepInfo.GetType().Name[..^4])
                this.MyStepInfo.title = funcModel.name;
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
            this.FunctionSelector.gameObject.SetActive(false);
            this.ExtensionHeight = 0;
            this.ClearLink();
            this.ClearSlots();
            this.BuildSlots();
            this.BuildLink();
            this.InoutContainerPlane.rectTransform.sizeDelta = new Vector2(
                this.InoutContainerPlane.rectTransform.sizeDelta.x,
                this.InoutContainerPlane.rectTransform.sizeDelta.y + oriExtensionHeight
                );
            this.RefreshRectTransform();
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
