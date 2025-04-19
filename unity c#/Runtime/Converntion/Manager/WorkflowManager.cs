using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Convention.WindowsUI.Variant;
using Unity.Collections.LowLevel.Unsafe;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.InputSystem;

namespace Convention.Workflow
{
    [Serializable,ArgPackage]
    public class NodeResult
    {
        public int nodeID;
        public string nodeTitle;
        // context_type or context_value_type
        public object result;
    }

    [Serializable,ArgPackage]
    public class ContextResult
    {
        public string hashID;
        public List<NodeResult> results;
        public float progress;
        public int task_count;
    }

    [Serializable, ArgPackage]
    public class FunctionModel
    {
        public string name = "unknown";
        public string description = "unknown";
        public Dictionary<string, string> parameters = new();
        public Dictionary<string, string> returns = new();
    }

    [Serializable, ArgPackage]
    public class Workflow : AnyClass
    {
        public List<NodeInfo> Datas = new();
        [NonSerialized] public List<Node> Nodes = new();
        public List<FunctionModel> Functions = new();
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
        [Setting, HopeNotNull] public ScriptableObject TextLabels;

        //[Resources, SerializeField, OnlyNotNullMode, Header("Prefabs")]
        //private GameObject GraphNodePrefab;
        [Resources, SerializeField, OnlyNotNullMode, Header("Content")]
        public RectTransform ContentPlane;
        [Resources, SerializeField, OnlyNotNullMode, Header("Mouse Click")]
        public RectTransform focusObject;
        [SerializeField, OnlyNotNullMode]
        public RectTransform UIFocusObject;
        private List<SharedModule.CallbackData> callbackDatas = new();
        private HashSet<Type> registeredCallbackNodeType = new();

        public List<FunctionModel> CallableFunctionModels = new();

        public void RegisterFunctionModel([In] FunctionModel func)
        {
            CallableFunctionModels.Add(func);
            Debug.Log($"[{String.Join(", ", func.returns.ToList().ConvertAll(x => $"{x.Key}: {x.Value}"))}] {func.name}" +
                $"({String.Join(", ", func.parameters.ToList().ConvertAll(x => $"{x.Key}: {x.Value}"))}): {func.description}");
        }
        public void UnregisterFunctionModel([In]FunctionModel func)
        {
            CallableFunctionModels.Remove(func);
        }
        public List<string> GetAllFunctionName()
        {
            return CallableFunctionModels.ConvertAll(x => x.name);
        }
        public bool ContainsFunctionModel(string functionName)
        {
            return CallableFunctionModels.Any(x => x.name == functionName);
        }
        [return:ReturnMayNull]
        public FunctionModel GetFunctionModel(string functionName)
        {
            return CallableFunctionModels.FirstOrDefault(x => x.name == functionName);
        }

        public delegate bool CustomTransformer([In] string word, out string late);
        public List<CustomTransformer> customTransformers = new();
        public static string Transformer([In] string str)
        {
            if(string.IsNullOrEmpty(str))
                return str;
            if (instance != null)
            {
                foreach (var customTransformer in instance.customTransformers)
                {
                    if (customTransformer(str, out var result))
                    {
                        var menuButton = instance.callbackDatas.Find(x => x.name == str);
                        if (menuButton != null)
                        {
                            menuButton.name = result;
                        }
                        return result;
                    }
                }
                if (instance.TextLabels == null)
                    return str;
                if (instance.TextLabels.symbols.ContainsKey(str))
                    return instance.TextLabels.symbols[str];
            }
            return str;
        }

        public void SetupWorkflowGraphNodeType([In] SharedModule.CallbackData callback)
        {
            callbackDatas.Add(callback);
        }
        public void SetupWorkflowGraphNodeType(params NodeInfo[] templates)
        {
            foreach (NodeInfo nodeInfo in templates)
            {
                if (registeredCallbackNodeType.Contains(nodeInfo.GetType()))
                    continue;
                registeredCallbackNodeType.Add(nodeInfo.GetType());
                string label = nodeInfo.GetType().Name;
                if (label.EndsWith("Info"))
                    label = label[..^4];
                SetupWorkflowGraphNodeType(new SharedModule.CallbackData(Transformer(label), x =>
                {
                    var info = nodeInfo.TemplateClone();
                    var node = this.CreateGraphNode(info);
                    var pos = focusObject.position;
                    node.transform.position = new Vector2(pos.x, pos.y);
                }));
            }
        }

        private void Start()
        {
            callbackDatas = new();
            m_RegisterWrapper = new(() =>
            {
                Debug.Log($"{nameof(WorkflowManager)} registered");
                if (GraphNodePrefabs == null)
                    GraphNodePrefabs = Resources.Load<ScriptableObject>("Workflow/Nodes");
                SetupWorkflowGraphNodeType(
                    new TextNodeInfo(),
                    new ValueNodeInfo(),
                    new ResourceNodeInfo(),
                    new StepNodeInfo(),
                    new EndNodeInfo());
            }, typeof(GraphInputWindow), typeof(GraphInspector));
#if UNITY_EDITOR
            if (this.GetAllFunctionName().Count() == 0) 
                this.RegisterFunctionModel(new()
                {
                    name = "TestFunction",
                    parameters =
                    {
                        {
                            "In", "string"
                        }
                    },
                    returns =
                    {
                        {
                            "Out", "string"
                        }
                    }
                });
#endif
        }

        private void Update()
        {
            if (Keyboard.current[Key.LeftCtrl].isPressed)
            {
                var t = -Mouse.current.scroll.y.ReadValue() * ScrollSpeed * 0.001f;
                var z = m_CameraTransform.transform.localPosition.z;
                if (z - t > -100 && z - t < -5)
                    m_CameraTransform.transform.Translate(new Vector3(0, 0, -t), Space.Self);
            }
            UIFocusObject.position = Mouse.current.position.ReadValue();
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
            node.gameObject.SetActive(true);
            node.transform.SetParent(ContentPlane);
            node.transform.localScale = Vector3.one;
            node.transform.eulerAngles = Vector3.zero;
            workflow.Nodes.Add(node);
            node.BuildSlots();
            node.MyNodeTab = GraphInputWindow.instance.RegisterOnHierarchyWindow(node.info);
            return node;
        }
        public bool DestroyNode(Node node)
        {
            int id = this.GetGraphNodeID(node);
            if (id >= 0)
            {
                workflow.Nodes.RemoveAt(id);
                workflow.Datas.Remove(node.info);
                GameObject.Destroy(node.gameObject);
            }
            else
            {
                Debug.LogError("node is not in current workflow");
            }
            return false;
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

        public void SaveWorkflowWithSystemPlugin()
        {
            SaveWorkflow(PluginExtenion.SaveFile("工作流|*.workflow;*.json", "保存工作流"));
        }
        public void LoadWorkflowWithSystemPlugin()
        {
            LoadWorkflow(PluginExtenion.SelectFile("工作流|*.workflow;*.json", "加载工作流"));
        }

        [Content, OnlyPlayMode] public string LastSavePath = null;

        public ToolFile SaveWorkflow(string workflowPath)
        {
            LastSavePath = workflowPath;
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
            ConventionUtility.CreateSteps().Next(()=>
            {
                foreach (var node in this.m_workflow.Nodes)
                {
                    node.BuildLink();
                }
            }).Invoke();
            return this.workflow;

        }
        public Workflow LoadWorkflow(string workflowPath)
        {
            ToolFile local = new(workflowPath);
            if (local.IsExist == false)
                throw new FileNotFoundException($"{local} is not exist");
            LastSavePath = workflowPath;
            var loadedWorkflow = (Workflow)local.LoadAsJson();
            return LoadWorkflow(loadedWorkflow);
        }
        public void OpenMenu(PointerEventData data)
        {
            focusObject.position = data.pointerCurrentRaycast.worldPosition;
#if UNITY_EDITOR
            if (callbackDatas.Count == 0)
                SharedModule.instance.OpenCustomMenu(UIFocusObject, new SharedModule.CallbackData("Empty", x => Debug.Log(x)));
            else
#endif
            {
                SharedModule.instance.OpenCustomMenu(UIFocusObject, callbackDatas.ToArray());
            }
        }
    }
}
