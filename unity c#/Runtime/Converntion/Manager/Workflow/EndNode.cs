using System;
using System.Collections;
using System.Collections.Generic;
using Convention.WindowsUI.Variant;
using UnityEngine;

namespace Convention.Workflow
{
    public class EndNode : Node
    {
        internal static List<EndNode> AllEndNodes = new();

        // ContextBehaviour

        public object end_result;

        protected override void Start()
        {
            base.Start();
            end_result = null;
            AllEndNodes.Add(this);
        }
        private void OnDestroy()
        {
            AllEndNodes.Remove(this);
        }

        private Dictionary<string, PropertiesWindow.ItemEntry> m_dynamicSlots = new();

        public bool AddSlot(string name, string typeIndicator)
        {
            if (this.info.inmapping.ContainsKey(name))
                return false;
            this.info.inmapping[name] = new NodeSlotInfo()
            {
                parentNode = this,
                slotName = name,
                typeIndicator = typeIndicator,
                IsInmappingSlot = true
            };
            var entry = CreateGraphNodeSlots(1)[0];
            m_dynamicSlots.Add(name, entry);
            this.m_Inmapping[name] = entry.ref_value.GetComponent<NodeSlot>();
            this.m_Inmapping[name].SetupFromInfo(this.info.inmapping[name]);
            return true;
        }

        public bool RemoveSlot(string name)
        {
            if(this.info.inmapping.ContainsKey(name)==false)
                return false;
            this.info.inmapping.Remove(name);
            this.m_Inmapping.Remove(name);
            m_dynamicSlots[name].Release();
            m_dynamicSlots.Remove(name);
            return true;
        }
    }
}
