using System;
using System.Linq;
using UnityEngine;

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
        [Resources, SerializeField, OnlyNotNullMode, Header("Only One Output")] private NodeSlot OutputSlot;
        public override void BuildSlots()
        {
            
        }

        public override void ClearSlots()
        {
            
        }

        protected override void WhenSetup(NodeInfo info)
        {
            base.WhenSetup(info);
            var pair = info.outmapping.First();
            m_Outmapping.Clear();
            var slotInfo = pair.Value.TemplateClone();
            slotInfo.parentNode = this;
            OutputSlot.SetupFromInfo(slotInfo);
            m_Outmapping[pair.Key] = OutputSlot;
        }
    }
}
