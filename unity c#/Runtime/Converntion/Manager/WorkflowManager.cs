using System;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
using UnityEngine.InputSystem;

namespace Convention.Workflow
{
    [Serializable, ArgPackage]
    public class Workflow : AnyClass
    {
        public List<GraphNodeInfo> Datas;
        [NonSerialized] public List<GraphNode> Nodes;
    }

    public class WorkflowManager : MonoSingleton<WorkflowManager>
    {
        public static float CameraZ = 0;

        [Content] public Workflow m_workflow;
        public Workflow workflow
        {
            get
            {
                return m_workflow;
            }
            set
            {
                if (m_workflow == value)
                    return;
                ClearWorkflowGraph();
                m_workflow = value;
                BuildWorkflowGraph();
            }
        }
        private RegisterWrapper<WorkflowManager> m_RegisterWrapper;
        [Resources, OnlyNotNullMode, SerializeField] private Transform m_CameraTransform;
        [Setting] public float ScrollSpeed = 1;

        [Resources, SerializeField, OnlyNotNullMode, Header("Prefabs")]
        private ScriptableObject GraphNodePrefabs;
        [Resources, SerializeField, OnlyNotNullMode, Header("Content")]
        private RectTransform ContentPlane;

        private void Start()
        {
            m_RegisterWrapper = new(() =>
            {

            }, typeof(GraphInputWindow), typeof(GraphInspector));
        }

        private void Update()
        {
            if (Keyboard.current[Key.LeftCtrl].isPressed)
            {
                var t = Mouse.current.scroll.y.ReadValue() * ScrollSpeed * 0.001f;
                var z = m_CameraTransform.transform.localPosition.z;
                if (z - t > -100 && z - t < -5)
                    m_CameraTransform.transform.Translate(new Vector3(0, 0, -t), Space.Self);
            }
        }

        private void LateUpdate()
        {
            CameraZ = Camera.main.transform.position.z;
        }

        public void RefreshImmediate()
        {
            ClearWorkflowGraph();
            BuildWorkflowGraph();
        }
        public void ClearWorkflowGraph()
        {
            foreach (var node in workflow.Nodes)
            {
                GameObject.Destroy(node.gameObject);
            }
            workflow.Nodes.Clear();
            workflow.Datas.Clear();
        }
        public void BuildWorkflowGraph()
        {
            foreach (var node in workflow.Nodes)
            {
                node.RefreshImmediate();
            }
        }

        public int CreateGraphNodeWithSetup([In]GraphNodeInfo info)
        {
            var node = GameObject.Instantiate((GraphNodePrefabs.Datas[info.GraphNodeType].uobjectValue as GameObject), ContentPlane).GetComponent<GraphNode>();
            node.info = info;
            workflow.Nodes.Add(node);
            workflow.Datas.Add(info);
            return workflow.Nodes.Count;
        }
        public bool ContainsNode(int id)
        {
            if (id < 0)
                return false;
            return workflow.Nodes.Count < id;
        }
        public GraphNode GetGraphNode(int id)
        {
            if (id < 0)
                return null;
            return workflow.Nodes[id];
        }
        public GraphNodeInfo GetGraphNodeInfo(int id)
        {
            if(id < 0)
                return null;
            return workflow.Nodes[id].info;
        }
        public int GetGraphNodeID(GraphNode node)
        {
            if (node == null)
                return -1;
            return workflow.Nodes.IndexOf(node);
        }

        public ToolFile SaveWorkflow(string workflowPath)
        {
            ToolFile local = new(workflowPath);
            ToolFile parent = local.GetParentDir();
            if (parent.IsExist == false)
                throw new FileNotFoundException($"{parent} is not exist");
            var currentWorkflow = workflow;
            currentWorkflow.Datas.Clear();
            for (int i = 0,e=currentWorkflow.Nodes.Count;i!=e;i++)
            {
                var node = currentWorkflow.Nodes[i];
                node.info.position = node.transform.position;
                currentWorkflow.Datas.Add(node.info);
            }
            local.data = currentWorkflow;
            local.SaveAsJson();
            return local;
        }
        public Workflow LoadWorkflow(string workflowPath)
        {
            ToolFile local = new(workflowPath);
            if (local.IsExist == false)
                throw new FileNotFoundException($"{local} is not exist");
            var loadedWorkflow = (Workflow)local.LoadAsJson();
            ClearWorkflowGraph();
            this.m_workflow = new();
            foreach (var info in loadedWorkflow.Datas)
            {
                CreateGraphNodeWithSetup(info);
            }
            return workflow;
        }
    }
}
