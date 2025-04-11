using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Convention.Workflow
{
    [Serializable, ArgPackage]
    public class ResourceNodeInfo : StartNodeInfo
    {
        public string resource = "unknown";

        public ResourceNodeInfo() : this(WorkflowManager.Transformer(nameof(ResourceNode))) { }
        public ResourceNodeInfo(string resource, string outmappingName = "value", int targetNodeID = -1, string targetSlotName = "value")
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
