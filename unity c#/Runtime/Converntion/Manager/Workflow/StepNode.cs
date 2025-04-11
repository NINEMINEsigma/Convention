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
                foreach (var (name, info) in WorkflowManager.instance.GetFunctionModel(x).Parameters)
                {
                    this.MyStepInfo.inmapping[name] = info.TemplateClone();
                }
                this.MyStepInfo.outmapping = new();
                foreach (var (name, info) in WorkflowManager.instance.GetFunctionModel(x).Returns)
                {
                    this.MyStepInfo.outmapping[name] = info.TemplateClone();
                }
                this.title = x;
                this.ClearLink();
                this.ClearSlots();
                this.BuildSlots();
                this.BuildLink();
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
                    FunctionSelector.CreateOption(funcName);
                }
            }
            else
            {
                FunctionSelector.CreateOption(WorkflowManager.instance.Transformer("No Function Registered"));
            }
        }
    }
}
