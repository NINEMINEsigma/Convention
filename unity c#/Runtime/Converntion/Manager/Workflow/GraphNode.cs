using System;
using System.Collections;
using System.Collections.Generic;
using Convention.WindowsUI;
using Convention.WindowsUI.Variant;
using UnityEngine;

namespace Convention.Workflow
{
    public class GraphNode : WindowsComponent, IOnlyFocusThisOnInspector, ITitle
    {
        [HideInInspector] public BehaviourContextManager Context;
        [Resources, OnlyNotNullMode, SerializeField] private Text Title;
        public virtual string GraphNodeTypeName => "Unknown";

        public string title
        {
            get => ((ITitle)this.Title).title;
            set => ((ITitle)this.Title).title = value;
        }

        [Serializable, ArgPackage]
        public class GraphNodeInfo : AnyClass
        {
            [Ignore,NonSerialized]private readonly GraphNode m_node;
            [SerializeField] private string InjectGraphNodeTitle = "";
            [SerializeField] private Dictionary<string, int> InjectGraphNodeInmapping = new();
            [SerializeField] private Dictionary<string, int> InjectGraphNodeOutmapping = new();

            public GraphNodeInfo(GraphNode node)
            {
                this.m_node = node;
                InjectGraphNodeTitle = node.title;
            }

            [InspectorDraw(InspectorDrawType.Text, name: "节点名称")]
            public string GraphNodeTitle
            {
                get => InjectGraphNodeTitle;
                set => InjectGraphNodeTitle = m_node.title = value;
            }
            [InspectorDraw(InspectorDrawType.Text, false, false, name: "节点类型")]
            public string GraphNodeType => m_node.GraphNodeTypeName;

        }
        public GraphNodeInfo m_info;



        private void Start()
        {
            if (Context == null)
                Context = this.GetOrAddComponent<BehaviourContextManager>();
            m_info = new(this);
            Context.OnPointerDownEvent = BehaviourContextManager.InitializeContextSingleEvent(Context.OnPointerDownEvent, _ =>
            {
                InspectorWindow.instance.SetTarget(this.m_info, null);
            });
        }
    }
}
