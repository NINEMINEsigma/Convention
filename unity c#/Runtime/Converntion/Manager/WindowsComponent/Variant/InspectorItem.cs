using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using UnityEngine;
using UnityEngine.Rendering;

namespace Convention.WindowsUI.Variant
{
    public enum InspectorDrawType
    {
        // String
        Text = 0,
        // Bool
        Toggle = 1,
        // Sripte
        Image = 2,
        // Transform
        Transform = 3,
        // Container
        List = 4, Dictionary = 5, Array = 6,
        // Object
        Reference = 7, Structure = 8,
        // Method
        Button = 9
    }


    [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property | AttributeTargets.Method, Inherited = true, AllowMultiple = false)]
    public class InspectorDrawAttribute : Attribute
    {
        public readonly InspectorDrawType drawType;
        public InspectorDrawAttribute(InspectorDrawType drawType)
        {
            this.drawType = drawType;
        }
    }

    public abstract class InspectorDrawer : MonoAnyBehaviour
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
        private InspectorDrawer m_TextModule, m_ToggleModule, m_ImageModule, m_ListModule,
            m_DictionaryModule, m_ArrayModule, m_ReferenceModule, m_StructureModule, m_ButtonModule;
        private Dictionary<InspectorDrawType, InspectorDrawer> m_AllUIModules = new();

        [Content, OnlyPlayMode] public IAnyClass target;
        [Content, OnlyPlayMode] public MemberInfo targetMemberInfo;
        [Content, OnlyPlayMode, SerializeField] private InspectorDrawType targetDrawType;
        [Content, OnlyPlayMode, SerializeField] private bool targetAbleChangeMode = true;
        [Content, OnlyPlayMode, SerializeField] private bool targetUpdateMode = false;

        public InspectorDrawer CurrentModule => m_AllUIModules[targetDrawType];
        public InspectorDrawType DrawType
        {
            get => targetDrawType;
            set
            {
                m_AllUIModules[targetDrawType].gameObject.SetActive(false);
                targetDrawType = value;
                m_AllUIModules[targetDrawType].gameObject.SetActive(true);
            }
        }
        public bool AbleChangeType
        {
            get => targetAbleChangeMode;
            private set => targetAbleChangeMode = value;
        }
        public bool UpdateType
        {
            get => targetUpdateMode;
            private set => targetUpdateMode = value;
        }

        public static string BroadcastName => $"On{nameof(InspectorItem)}Init";

        protected override void Start()
        {
            base.Start();
            m_AllUIModules[InspectorDrawType.Text] = m_TextModule;
            m_AllUIModules[InspectorDrawType.Toggle] = m_ToggleModule;
            m_AllUIModules[InspectorDrawType.Image] = m_ImageModule;
            m_AllUIModules[InspectorDrawType.Transform] = m_TransformModule;
            m_AllUIModules[InspectorDrawType.List] = m_ListModule;
            m_AllUIModules[InspectorDrawType.Dictionary] = m_DictionaryModule;
            m_AllUIModules[InspectorDrawType.Array] = m_ArrayModule;
            m_AllUIModules[InspectorDrawType.Reference] = m_ReferenceModule;
            m_AllUIModules[InspectorDrawType.Structure] = m_StructureModule;
            m_AllUIModules[InspectorDrawType.Button] = m_ButtonModule;
            foreach (var module in m_AllUIModules)
            {
                module.Value.OnInspectorItemInit(this);
            }
        }

        public void SetTarget([In] IAnyClass target, MemberInfo member)
        {
            this.target = target;
            this.targetMemberInfo = member;
            RebulidImmediate();
        }

        public void SetValue([In] object value)
        {
            ConventionUtility.PushValue(target, value, targetMemberInfo);
        }
        public object GetValue()
        {
            return ConventionUtility.SeekValue(target, targetMemberInfo);
        }
        public void InvokeMember()
        {
            ConventionUtility.InvokeMember(targetMemberInfo, target);
        }

        [Content, OnlyPlayMode]
        public void RebulidImmediate()
        {
            InspectorDrawAttribute drawAttr = null;
            ArgPackageAttribute argAttr = null;
            Type type = null;
            string name = null;
            // Reset AbleChangeType
            var info = targetMemberInfo;
            drawAttr = info.GetCustomAttribute<InspectorDrawAttribute>(true);
            argAttr = info.GetCustomAttribute<ArgPackageAttribute>(true);
            type = ConventionUtility.GetMemberValue(info);
            name = info.Name;
            AbleChangeType = info.GetCustomAttributes(typeof(IgnoreAttribute), true).Length == 0;
            // Reset DrawType
            if (drawAttr != null)
            {
                DrawType = drawAttr.drawType;
            }
            else if (type != null)
            {
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
}
