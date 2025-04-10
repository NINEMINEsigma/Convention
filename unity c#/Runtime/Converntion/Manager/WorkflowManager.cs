using System;
using System.Collections.Generic;
using System.IO;
using Convention.WindowsUI.Variant;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.InputSystem;

namespace Convention.Workflow
{
    [Serializable, ArgPackage]
    public class Workflow : AnyClass
    {
        public List<NodeInfo> Datas;
        [NonSerialized] public List<Node> Nodes;
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
        [Setting, OnlyNotNullMode] public ScriptableObject GraphNodePrefabs;
        [Setting, OnlyNotNullMode] public ScriptableObject TextLabels;

        //[Resources, SerializeField, OnlyNotNullMode, Header("Prefabs")]
        //private GameObject GraphNodePrefab;
        [Resources, SerializeField, OnlyNotNullMode, Header("Content")]
        private RectTransform ContentPlane;
        [Resources, SerializeField, OnlyNotNullMode, Header("Mouse Click")]
        private RectTransform focusObject;
        private List<SharedModule.CallbackData> callbackDatas = new();

        public string Transformer([In] string str)
        {
            if (TextLabels.Datas.ContainsKey(str))
                return TextLabels.Datas[str].stringValue;
            return str;
        }

        public void SetupWorkflowGraphNodeType([In] SharedModule.CallbackData callback)
        {
            callbackDatas.Add(callback);
        }
        public void SetupWorkflowGraphNodeType([In]string menu, [In] string label, [In] NodeInfo template)
        {
            SetupWorkflowGraphNodeType(new(menu, x =>
            {
                SharedModule.instance.OpenCustomMenu(focusObject, new SharedModule.CallbackData(label, y =>
                {
                    var info = template.TemplateClone();
                    var node = CreateGraphNode(info);
                    node.transform.position = y;
                }));
            }));
        }

        private void Start()
        {
            m_RegisterWrapper = new(() =>
            {
                if (GraphNodePrefabs == null)
                    GraphNodePrefabs = Resources.Load<ScriptableObject>("Workflow/Nodes");
                SetupWorkflowGraphNodeType(Transformer(nameof(StartNode)), Transformer(nameof(TextNode)), new TextNodeInfo(Transformer("Text")));
                SetupWorkflowGraphNodeType(Transformer(nameof(StartNode)), Transformer(nameof(ResourceNode)), new ResourceNodeInfo() { resource = Transformer("Path or URL") });
                SetupWorkflowGraphNodeType(Transformer(nameof(StepNode)), Transformer(nameof(StepNode)), new StepNodeInfo() { funcname = Transformer("FunctionName") });
                SetupWorkflowGraphNodeType(Transformer(nameof(EndNode)), Transformer(nameof(EndNode)), new EndNodeInfo());
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
            foreach (var node in workflow.Nodes)
            {
                node.RefreshImmediate();
            }
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
            foreach (var info in workflow.Datas)
            {
                CreateGraphNode(info);
            }
        }

        public Node CreateGraphNode([In] NodeInfo info)
        {
            var node = info.Instantiate();
            workflow.Nodes.Add(node);
            node.BuildSlots();
            return node;
        }
        public bool DestroyNode(Node node)
        {
            int id = this.GetGraphNodeID(node);
            if (id >= 0)
            {
                workflow.Nodes.RemoveAt(id);
            }
            return workflow.Nodes.Remove(node);
        }
        public bool ContainsNode(int id)
        {
            if (id < 0)
                return false;
            return workflow.Nodes.Count < id;
        }
        public Node GetGraphNode(int id)
        {
            if (id < 0)
                return null;
            return workflow.Nodes[id];
        }
        public int GetGraphNodeID(Node node)
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
            foreach(var node in currentWorkflow.Nodes)
            {
                node.info.CopyFromNode(node);
                currentWorkflow.Datas.Add(node.info);
            }
            local.data = currentWorkflow;
            local.SaveAsJson();
            return local;
        }
        public Workflow LoadWorkflow(Workflow workflow)
        {
            ClearWorkflowGraph();
            workflow.Datas.Sort((x, y) => x.nodeID.CompareTo(y.nodeID));
            for (int i = 0; i < workflow.Datas.Count; i++)
            {
                if (workflow.Datas[i].nodeID != i)
                    throw new InvalidOperationException("Bad workflow: nodeID != node index");
            }    
            this.m_workflow = new();
            foreach (var info in workflow.Datas)
            {
                CreateGraphNode(info);
            }
            foreach (var node in workflow.Nodes)
            {
                node.BuildLink();
            }
            return this.workflow;

        }
        public Workflow LoadWorkflow(string workflowPath)
        {
            ToolFile local = new(workflowPath);
            if (local.IsExist == false)
                throw new FileNotFoundException($"{local} is not exist");
            var loadedWorkflow = (Workflow)local.LoadAsJson();
            return LoadWorkflow(loadedWorkflow);
        }

        public void OpenMenu(PointerEventData data)
        {
            focusObject.position = Mouse.current.position.ReadValue();
#if UNITY_EDITOR
            if (callbackDatas.Count == 0)
                SharedModule.instance.OpenCustomMenu(focusObject, new SharedModule.CallbackData("Empty", x => Debug.Log(x)));
            else
#endif
            {
                SharedModule.instance.OpenCustomMenu(focusObject, callbackDatas.ToArray());
            }
        }


    }
}
