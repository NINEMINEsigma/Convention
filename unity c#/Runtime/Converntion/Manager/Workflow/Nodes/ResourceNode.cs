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
    }

    public class ResourceNode : StartNode
    {

    }
}
