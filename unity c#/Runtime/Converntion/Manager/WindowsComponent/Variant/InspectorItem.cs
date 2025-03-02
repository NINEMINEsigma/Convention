using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using UnityEngine;
using UnityEngine.UI;

namespace Convention.WindowsUI.Variant
{
    public class InspectorItem : PropertyListItem
    {
        [Resources, OnlyNotNullMode, SerializeField, Header("Inspector Components")] private WindowUIModule m_TransformModule;
        [Resources, OnlyNotNullMode, SerializeField] private WindowUIModule m_TitleModule;
        [Resources, OnlyNotNullMode, SerializeField] private WindowUIModule  m_TextModule;
        [Resources,OnlyNotNullMode,SerializeField]private WindowUIModule m_ImageModule;
        [Resources,OnlyNotNullMode,SerializeField]private WindowUIModule m_InputFieldModule;
        [Resources, OnlyNotNullMode, SerializeField] private WindowUIModule m_ToggleModule;
        [Resources, OnlyNotNullMode, SerializeField] private WindowUIModule m_Vector2;
        [Resources, OnlyNotNullMode, SerializeField] private WindowUIModule m_Vector3;

        [Content, OnlyPlayMode] public IAnyClass target;
        [Content, OnlyPlayMode] public string valueName;

        private void SetValue([In]object value)
        {
            ConventionUtility.PushValue(target, value, valueName, BindingFlags.Public | BindingFlags.NonPublic);
        }

        protected override void Start()
        {
            base.Start();

        }

        protected override void OnEnable()
        {
            base.OnEnable();
            object value = ConventionUtility.SeekValue(target, valueName, BindingFlags.Public | BindingFlags.NonPublic, out bool isSucceed);
            //(m_TitleModule as ITitle).title=
        }
    }
}
