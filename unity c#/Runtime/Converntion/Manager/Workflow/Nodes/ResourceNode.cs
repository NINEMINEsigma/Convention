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
        protected override NodeInfo CreateTemplateNodeInfoBySelfType()
        {
            return new ResourceNodeInfo();
        }
    }

    public class ResourceNode : StartNode
    {
        //TODO
    }
}
