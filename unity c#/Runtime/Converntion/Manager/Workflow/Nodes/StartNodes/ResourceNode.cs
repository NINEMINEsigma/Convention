using System;
using System.Collections;
using System.Collections.Generic;
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

        public ResourceNodeInfo() : this(WorkflowManager.Transformer(nameof(ResourceNode))) { }
        public ResourceNodeInfo(string resource, string outmappingName = "value", int targetNodeID = -1, string targetSlotName = "value")
        {
            this.resource = resource;
            this.outmapping = new()
            {
                {
                    WorkflowManager.Transformer(outmappingName), new NodeSlotInfo()
                    {
                        slotName = WorkflowManager.Transformer(outmappingName),
                        typeIndicator = "string",
                        IsInmappingSlot = false,
                        targetNodeID = targetNodeID,
                        targetSlotName = targetSlotName,
                    }
                }
            };
        }
        protected override NodeInfo CreateTemplateNodeInfoBySelfType()
        {
            return new ResourceNodeInfo()
            {
                resource = resource
            };
        }
    }

    public class ResourceNode : StartNode
    {
        //TODO
    }
}
