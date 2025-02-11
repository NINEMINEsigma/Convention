using System;
using System.Collections.Generic;
using System.Reflection;
using UnityEditor;
using UnityEngine;

namespace Convention
{
#if false
    [CustomEditor(typeof(DataEntry))]
    public class DataEntryEditor : Editor
    {
        protected GUISkin customSkin;
        protected Color defaultColor;

        public bool Toggle(bool value)
        {
            GUILayout.BeginHorizontal(EditorStyles.helpBox);

            value = GUILayout.Toggle(value, new GUIContent(name), customSkin.FindStyle("Toggle"));
            value = GUILayout.Toggle(value, new GUIContent(""), customSkin.FindStyle("Toggle Helper"));

            GUILayout.EndHorizontal();
            return value;
        }
        public override void OnInspectorGUI()
        {
            serializedObject.Update();

            defaultColor = GUI.color;

            if (EditorGUIUtility.isProSkin == true)
                customSkin = (GUISkin)Resources.Load("Editor\\ADUI Skin Dark");
            else
                customSkin = (GUISkin)Resources.Load("Editor\\ADUI Skin Light");

            var that = target as DataEntry;
            var data = that.RealData;
            if (ConventionUtility.IsBool(data))
            {
                bool value = that.boolValue;
                bool cvalue = Toggle(value);
                if (value != cvalue)
                {
                    that.boolValue = cvalue;
                }
            }
            else if (ConventionUtility.IsNumber(data) || ConventionUtility.IsString(data))
            {
                string str = that.stringValue;
                string cstr = EditorGUILayout.TextField("Data", str);
                if (cstr != str)
                {
                    that.stringValue = str;
                }
            }
            else
            {

            }

            serializedObject.ApplyModifiedProperties();
        }
    }
#endif
    [CustomEditor(typeof(Convention.ScriptableObject))]
    public class ScriptableObjectEditor : AbstractCustomEditor
    {
        protected override void Field(FieldInfo field)
        {
            if (field.FieldType == typeof(bool))
                this.Toggle(field.Name);
            else
            {
                var p = serializedObject.FindProperty(field.Name);
                if (p != null)
                    EditorGUILayout.PropertyField(p);
            }
        }
    }
}
