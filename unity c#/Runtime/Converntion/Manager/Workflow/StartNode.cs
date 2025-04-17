using System;

namespace Convention.Workflow
{
    [Serializable, ArgPackage]
    public class StartNodeInfo : NodeInfo
    {
        protected override NodeInfo CreateTemplateNodeInfoBySelfType()
        {
            return new StartNodeInfo();
        }
    }

    public class StartNode : Node
    {

    }
}
