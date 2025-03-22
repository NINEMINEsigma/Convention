using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using UnityEngine;
using static Convention.WindowsUI.Variant.PropertiesWindow;

namespace Convention.WindowsUI.Variant
{
    /// <summary>
    /// enum&1==1则为动态生成类型
    /// </summary>
    public enum InspectorDrawType
    {
        // Auto
        Auto = -1,
        // String
        Text = 0,
        // Bool
        Toggle = 1 << 1,
        // Sripte
        Image = 1 << 2,
        // Transform
        Transform = 1 << 3,
        // Container
        List = 1 << 4 + 1, Dictionary = 1 << 5 + 1, Array = 1 << 6 + 1,
        // Object
        Reference = 1 << 7, Structure = 1 << 8,
        // Method
        Button = 1 << 9,
        // Enum
        Enum = 1 << 10
    }


    [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property | AttributeTargets.Method, Inherited = true, AllowMultiple = false)]
    public class InspectorDrawAttribute : Attribute
    {
        public readonly InspectorDrawType drawType;
        public InspectorDrawAttribute(InspectorDrawType drawType = InspectorDrawType.Auto)
        {
            this.drawType = drawType;
        }
    }

    public interface IInspectorUpdater
    {
        void OnInspectorUpdate();
    }

    public abstract class InspectorDrawer : WindowUIModule
    {
        public InspectorItem targetItem { get; private set; }
        public virtual void OnInspectorItemInit(InspectorItem item)
        {
            targetItem = item;
        }
    }

    public class InspectorItem : PropertyListItem
    {
        [Resources, OnlyNotNullMode, SerializeField, Header("Inspector Components")]
        private InspectorDrawer m_TransformModule;
        [Resources, OnlyNotNullMode, SerializeField]
        private InspectorDrawer m_TextModule, m_ToggleModule, m_ImageModule, m_ReferenceModule, 
            m_ButtonModule, m_StructureModule, m_DictionaryItemModule, m_EnumItemModule;
        private Dictionary<InspectorDrawType, InspectorDrawer> m_AllUIModules = new();
        private List<ItemEntry> m_DynamicSubEntries = new();

        [Content, OnlyPlayMode] public object target;
        public MemberInfo targetMemberInfo { get; private set; }
        public ValueWrapper targetValueWrapper { get; private set; }
        public Action targetFunctionCall { get; private set; }
        [Setting, SerializeField] private InspectorDrawType targetDrawType;
        [Setting, SerializeField] private bool targetAbleChangeMode = true;
        [Setting, SerializeField] private bool targetUpdateMode = true;

        public InspectorDrawer CurrentModule => m_AllUIModules[targetDrawType];
        public InspectorDrawType DrawType
        {
            get => targetDrawType;
            set
            {
                DisableDrawType();
                targetDrawType = value;
                EnableDrawType();
            }
        }

        private void EnableDrawType()
        {
            if ((1 & (int)targetDrawType) == 0)
                m_AllUIModules[targetDrawType].gameObject.SetActive(true);
            else if (targetDrawType == InspectorDrawType.List)
            {
                Type listType = (targetMemberInfo != null)
                    ? ConventionUtility.SeekValue(target, targetMemberInfo).GetType().GetGenericArguments()[0]
                    : targetValueWrapper.GetValue().GetType().GetGenericArguments()[0];
                m_DynamicSubEntries = CreateSubPropertyItem(2);
                m_DynamicSubEntries[0].ref_value.GetComponent<InspectorItem>().SetTarget(null, () =>
                {
                    (GetValue() as IList).Add(ConventionUtility.GetDefault(listType));
                });
                m_DynamicSubEntries[1].ref_value.GetComponent<InspectorItem>().SetTarget(null, () =>
                {
                    var list = (IList)GetValue();
                    int length = list.Count;
                    if (length != 0)
                        (GetValue() as IList).RemoveAt(length - 1);
                });
                CreateSequenceItems(2);
            }
            else if (targetDrawType == InspectorDrawType.Dictionary)
            {

            }
            else if (targetDrawType == InspectorDrawType.Array)
            {
                m_DynamicSubEntries = new();
                CreateSequenceItems();
            }
            else
                throw new InvalidOperationException($"Unknown {nameof(InspectorDrawType)}: {targetDrawType}");

            void CreateSequenceItems(int offset = 0)
            {
                var array = (IList)GetValue();
                Type arrayType = (targetMemberInfo != null)
                    ? ConventionUtility.SeekValue(target, targetMemberInfo).GetType().GetGenericArguments()[0]
                    : targetValueWrapper.GetValue().GetType().GetGenericArguments()[0];
                int length = array.Count;// (int)ConventionUtility.SeekValue(array, nameof(Array.Length), BindingFlags.Default);
                m_DynamicSubEntries.AddRange(CreateSubPropertyItem(length));
                int index = 0;
                foreach (var item in array)
                {
                    m_DynamicSubEntries[index + offset].ref_value.GetComponent<PropertyListItem>().title = index.ToString();
                    m_DynamicSubEntries[index + offset].ref_value.GetComponent<InspectorItem>().SetTarget(null, new ValueWrapper(
                        () => array[index],
                        (x) => array[index] = x,
                        arrayType
                        ));
                    index++;
                }
            }
        }

        private void DisableDrawType()
        {
            if ((1 & (int)targetDrawType) == 0)
                m_AllUIModules[targetDrawType].gameObject.SetActive(false);
            else
            {
                foreach (var item in m_DynamicSubEntries)
                {
                    item.Release();
                }
                m_DynamicSubEntries.Clear();
            }
        }

        public bool AbleChangeType
        {
            get => targetAbleChangeMode;
            set => targetAbleChangeMode = value;
        }
        public bool UpdateType
        {
            get => targetUpdateMode;
            set => targetUpdateMode = value;
        }

        public static string BroadcastName => $"On{nameof(InspectorItem)}Init";

        private void InitModules()
        {
            m_AllUIModules[InspectorDrawType.Text] = m_TextModule;
            m_AllUIModules[InspectorDrawType.Toggle] = m_ToggleModule;
            m_AllUIModules[InspectorDrawType.Image] = m_ImageModule;
            m_AllUIModules[InspectorDrawType.Transform] = m_TransformModule;
            m_AllUIModules[InspectorDrawType.Reference] = m_ReferenceModule;
            m_AllUIModules[InspectorDrawType.Structure] = m_StructureModule;
            m_AllUIModules[InspectorDrawType.Button] = m_ButtonModule;
            m_AllUIModules[InspectorDrawType.Enum] = m_EnumItemModule;
            foreach (var module in m_AllUIModules)
            {
                module.Value.OnInspectorItemInit(this);
            }
        }

        public void SetTarget([In] object target, MemberInfo member)
        {
            this.target = target;
            this.targetMemberInfo = member;
            this.targetValueWrapper = null;
            this.targetFunctionCall = null;
            InitModules();
            RebulidImmediate();
        }
        public void SetTarget([In] object target,ValueWrapper wrapper)
        {
            this.target = target;
            this.targetMemberInfo = null;
            this.targetValueWrapper = wrapper;
            this.targetFunctionCall = null;
            InitModules();
            RebulidImmediate();
        }
        public void SetTarget([In] object target,Action action)
        {
            this.target = target;
            this.targetMemberInfo = null;
            this.targetValueWrapper = null;
            this.targetFunctionCall = action;
            InitModules();
            RebulidImmediate();
        }

        public void SetValue([In] object value)
        {
            if (targetMemberInfo != null)
                ConventionUtility.PushValue(target, value, targetMemberInfo);
            else if (targetValueWrapper != null)
                targetValueWrapper.SetValue(value);
            else
                throw new InvalidOperationException();
        }
        public object GetValue()
        {
            if (targetMemberInfo != null)
                return ConventionUtility.SeekValue(target, targetMemberInfo);
            else if (targetValueWrapper != null)
                return targetValueWrapper.GetValue();
            else
                throw new InvalidOperationException();
        }
        public Type GetValueType()
        {
            if (targetMemberInfo != null)
                return ConventionUtility.GetMemberValueType(targetMemberInfo);
            else if (targetValueWrapper != null)
                return targetValueWrapper.type;
            else
                throw new InvalidOperationException();
        }
        public void InvokeAction()
        {
            if (targetFunctionCall != null)
                targetFunctionCall.Invoke();
            else
                ConventionUtility.InvokeMember(targetMemberInfo, target);
        }

        [Content, OnlyPlayMode]
        public void RebulidImmediate()
        {
            if (targetMemberInfo != null)
            {
                RebuildWithMemberInfo();
            }
            else if (targetValueWrapper != null)
            {
                RebuildWithWrapper();
            }
            else if(targetFunctionCall != null)
            {
                RebuildWithFunctionCall();
            }

            void RebuildWithMemberInfo()
            {
                InspectorDrawAttribute drawAttr = null;
                ArgPackageAttribute argAttr = null;
                Type type = null;
                string name = null;
                // Reset AbleChangeType
                drawAttr = targetMemberInfo.GetCustomAttribute<InspectorDrawAttribute>(true);
                argAttr = targetMemberInfo.GetCustomAttribute<ArgPackageAttribute>(true);
                type = ConventionUtility.GetMemberValueType(targetMemberInfo);
                name = targetMemberInfo.Name;
                AbleChangeType = targetMemberInfo.GetCustomAttributes(typeof(IgnoreAttribute), true).Length == 0;
                // Reset DrawType
                if (drawAttr != null && drawAttr.drawType != InspectorDrawType.Auto)
                {
                    DrawType = drawAttr.drawType;
                }
                else if (type != null)
                {
                    if (ConventionUtility.IsEnum(type))
                        DrawType = InspectorDrawType.Enum;
                    if (ConventionUtility.IsBool(type))
                        DrawType = InspectorDrawType.Toggle;
                    else if (ConventionUtility.IsString(type) || ConventionUtility.IsNumber(type))
                        DrawType = InspectorDrawType.Text;
                    else if (ConventionUtility.IsArray(type))
                        DrawType = InspectorDrawType.Array;
                    else if (ConventionUtility.IsImage(type))
                        DrawType = InspectorDrawType.Image;
                    else if (type.GetInterface(nameof(IEnumerable)) != null && type.GetGenericArguments().Length == 1)
                        DrawType = InspectorDrawType.List;
                    else if (type.GetInterface(nameof(IEnumerable)) != null && type.GetGenericArguments().Length == 2)
                        DrawType = InspectorDrawType.Dictionary;
                    else if (type == typeof(Transform))
                        DrawType = InspectorDrawType.Transform;
                    else if (type.IsClass)
                        DrawType = InspectorDrawType.Reference;
                    else
                        DrawType = InspectorDrawType.Structure;
                }
                else if (targetMemberInfo is MethodInfo method)
                {
                    DrawType = InspectorDrawType.Button;
                }
                else
                {
                    throw new NotImplementedException();
                }
                RectTransformExtension.AdjustSizeToContainsChilds(transform as RectTransform);
                RectTransformExtension.AdjustSizeToContainsChilds(this.Entry.rootWindow.TargetWindowContent);
            }

            void RebuildWithWrapper()
            {
                Type type = targetValueWrapper.type;
                AbleChangeType = targetValueWrapper.IsChangeAble;
                // Reset DrawType
                if (ConventionUtility.IsBool(type))
                    DrawType = InspectorDrawType.Toggle;
                else if (ConventionUtility.IsString(type) || ConventionUtility.IsNumber(type))
                    DrawType = InspectorDrawType.Text;
                else if (ConventionUtility.IsArray(type))
                    DrawType = InspectorDrawType.Array;
                else if (type.GetInterface(nameof(IEnumerable)) != null && type.GetGenericArguments().Length == 1)
                    DrawType = InspectorDrawType.List;
                else if (type.GetInterface(nameof(IEnumerable)) != null && type.GetGenericArguments().Length == 2)
                    DrawType = InspectorDrawType.Dictionary;
                else if (type == typeof(Transform))
                    DrawType = InspectorDrawType.Transform;
                else if (type.IsSubclassOf(typeof(Texture)))
                    DrawType = InspectorDrawType.Image;
                else if (type.IsClass)
                    DrawType = InspectorDrawType.Reference;
                else
                    DrawType = InspectorDrawType.Structure;
                RectTransformExtension.AdjustSizeToContainsChilds(transform as RectTransform);
                RectTransformExtension.AdjustSizeToContainsChilds(this.Entry.rootWindow.TargetWindowContent);
            }

            void RebuildWithFunctionCall()
            {
                DrawType = InspectorDrawType.Button;
            }
        }

        protected override void FoldChilds()
        {
            base.FoldChilds();
            CurrentModule.gameObject.SetActive(false);
        }
        protected override void UnfoldChilds()
        {
            base.UnfoldChilds();
            CurrentModule.gameObject.SetActive(true);
        }

    }


    /// <summary>
    /// 使用这个接口, 将在GameObject被SetTarget在Inspector上时只展示这个类实例的内容,
    /// 而不会展示Components也不能通过<To GameObject>跳转到GameObject的Components列表,
    /// 见<see cref="InspectorWindow.BuildWindow"/>
    /// </summary>
    public interface IOnlyFocusThisOnInspector : IAnyClass { }
}
