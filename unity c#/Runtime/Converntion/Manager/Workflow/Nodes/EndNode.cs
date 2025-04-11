using System;
using System.Collections;
using System.Collections.Generic;
using Convention.WindowsUI;
using Convention.WindowsUI.Variant;
using UnityEngine;
using UnityEngine.EventSystems;

namespace Convention.Workflow
{
    [Serializable, ArgPackage]
    public class EndNodeInfo : NodeInfo
    {
        protected override NodeInfo CreateTemplateNodeInfoBySelfType()
        {
            return new EndNodeInfo();
        }
    }

    public class EndNode : Node, INodeSlotLinkable
    {
        internal static List<EndNode> AllEndNodes = new();

        // ContextBehaviour

        public object end_result;

        protected override void Start()
        {
            base.Start();
            end_result = null;
            AllEndNodes.Add(this);
            var context = gameObject.GetOrAddComponent<BehaviourContextManager>();
            context.OnPointerClickEvent = BehaviourContextManager.InitializeContextSingleEvent(context.OnPointerClickEvent, PointerRightClickAndOpenMenu);
        }
        private void OnDestroy()
        {
            AllEndNodes.Remove(this);
        }

        private Dictionary<string, PropertiesWindow.ItemEntry> m_dynamicSlots = new();

        public void PointerRightClickAndOpenMenu(PointerEventData pointer)
        {
            if (pointer.button == PointerEventData.InputButton.Right)
            {
                List<SharedModule.CallbackData> callbacks = new()
                {
                    new (WorkflowManager.instance.Transformer("Create New Slot"), x =>
                    {
                        string slotName="";
                        SharedModule.instance.SingleEditString(
                            WorkflowManager.instance.Transformer("SlotName"),
                            WorkflowManager.instance.Transformer("SlotName"),
                            y => slotName = y);
                        AddSlot(slotName,"string");
                    })
                };
                SharedModule.instance.OpenCustomMenu(WorkflowManager.instance.UIFocusObject, callbacks.ToArray());
            }
        }

        public bool AddSlot(string name, string typeIndicator)
        {
            if (this.m_Inmapping.ContainsKey(name))
                return false;
            var entry = CreateGraphNodeInSlots(1)[0];
            RectTransform curEntryRect = entry.ref_value.transform as RectTransform;
            this.m_Inmapping[name] = entry.ref_value.GetComponent<NodeSlot>();
            this.m_Inmapping[name].SetupFromInfo(new NodeSlotInfo()
            {
                parentNode = this,
                slotName = name,
                typeIndicator = typeIndicator,
                IsInmappingSlot = true
            });
            m_dynamicSlots.Add(name, entry);
            this.rectTransform.sizeDelta = new Vector2(this.rectTransform.sizeDelta.x, this.rectTransform.sizeDelta.y + curEntryRect.rect.height);
            return true;
        }

        public bool RemoveSlot(string name)
        {
            if (this.m_Inmapping.ContainsKey(name) == false)
                return false;
            this.m_Inmapping.Remove(name);
            RectTransform curEntryRect = m_dynamicSlots[name].ref_value.transform as RectTransform;
            this.rectTransform.sizeDelta = new Vector2(this.rectTransform.sizeDelta.x, this.rectTransform.sizeDelta.y - curEntryRect.rect.height);
            m_dynamicSlots[name].Release();
            m_dynamicSlots.Remove(name);
            return true;
        }

        public bool LinkTo([In, Opt] NodeSlot other)
        {
            if (Linkable(other))
            {
                AddSlot(other.info.slotName, other.info.typeIndicator);
                var slot = m_dynamicSlots[other.info.slotName].ref_value.GetComponent<NodeSlot>();
                if (slot.Linkable(other))
                {
                    slot.LinkTo(other);
                    return true;
                }
            }
            return false;
        }

        public bool Linkable([In] NodeSlot other)
        {
            return other != null && other.info.IsInmappingSlot == false;
        }
    }
}
