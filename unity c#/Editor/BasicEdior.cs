using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using Convention.Internal;
using UnityEditor;
using UnityEngine;
using UnityEngine.Events;

namespace Convention
{
    [CustomEditor(typeof(ConventionEditorInspectorGUI), true)]
    public class AbstractCustomEditor : Editor
    {
        protected int currentTab;

        protected virtual string TopHeader => "CM Top Header";

        protected void OnEnable()
        {

        }

        public void OnNotChangeGUI(UnityAction action)
        {
            GUI.enabled = false;
            action();
            GUI.enabled = true;
        }

        public void HelpBox(string message, MessageType messageType)
        {
            EditorGUILayout.HelpBox(message, messageType);
        }

        public void HorizontalBlock(UnityAction action)
        {
            GUILayout.BeginHorizontal();
            action();
            GUILayout.EndHorizontal();
        }

        public void HorizontalBlockWithBox(UnityAction action)
        {
            GUILayout.BeginHorizontal(EditorStyles.helpBox);
            action();
            GUILayout.EndHorizontal();
        }

        public void VerticalBlockWithBox(UnityAction action)
        {
            GUILayout.BeginVertical(EditorStyles.helpBox);
            action();
            GUILayout.EndVertical();
        }

        protected void IgnoreField(FieldInfo field)
        {
            this.OnNotChangeGUI(() => Field(field, false));
        }
        protected virtual void PlayModeField(FieldInfo field)
        {
            HelpBox($"{field.Name}<{field.FieldType}> only play mode", MessageType.Info);
        }
        protected virtual void Field(FieldInfo field, bool isCheckIgnore = true)
        {
            if (field.GetCustomAttributes(typeof(OnlyPlayModeAttribute),true).Length != 0 && Application.isPlaying == false)
            {
                PlayModeField(field);
            }
            else if (isCheckIgnore && (
                field.GetCustomAttributes(typeof(IgnoreAttribute), true).Length != 0 ||
                field.IsPublic == false
                ))
                IgnoreField(field);
            else if (field.FieldType == typeof(bool))
                this.Toggle(field.Name);
            else
            {
                var p = serializedObject.FindProperty(field.Name);
                var tfattr = field.GetCustomAttribute<ToolFile.FileAttribute>(true);
                if (tfattr != null)
                    GUILayout.BeginVertical(EditorStyles.helpBox);
                if (p == null)
                {
                    HelpBox($"{field.Name}<{field.FieldType}> cannt draw", MessageType.Warning);
                }
                else
                {
                    EditorGUILayout.PropertyField(p);
                    if (tfattr != null && field.FieldType == typeof(string))
                    {
                        if (GUILayout.Button("Browse"))
                            p.stringValue = ToolFile.BrowseFile("*");
                    }
                }
                if (tfattr != null)
                    GUILayout.EndHorizontal();
            }
        }
        public virtual void OnContentGUI()
        {
            var fields = this.target.GetType().GetFields(BindingFlags.Public | BindingFlags.NonPublic|BindingFlags.Instance);
            foreach (var field in fields)
            {
                bool isContent = field.GetCustomAttributes(typeof(ContentAttribute), true).Length != 0;
                bool isResources = field.GetCustomAttributes(typeof(ResourcesAttribute), true).Length != 0;
                bool isSetting = field.GetCustomAttributes(typeof(SettingAttribute), true).Length != 0;
                if (!isContent && (isResources || isSetting))
                    continue;
                if (!field.FieldType.IsSubclassOf(typeof(UnityEngine.Object)) || isContent)
                {
                    Field(field);
                }
            }
        }
        public virtual void OnResourcesGUI()
        {
            var fields = this.target.GetType().GetFields(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
            foreach (var field in fields)
            {
                bool isContent = field.GetCustomAttributes(typeof(ContentAttribute), true).Length != 0;
                bool isResources = field.GetCustomAttributes(typeof(ResourcesAttribute), true).Length != 0;
                bool isSetting = field.GetCustomAttributes(typeof(SettingAttribute), true).Length != 0;
                if (!isResources && (isContent || isSetting))
                    continue;
                if (field.FieldType.IsSubclassOf(typeof(UnityEngine.Object)) || isResources)
                {
                    Field(field);
                }
            }
        }
        public virtual void OnSettingsGUI()
        {
            var fields = this.target.GetType().GetFields(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
            foreach (var field in fields)
            {
                if (field.GetCustomAttributes(typeof(SettingAttribute), true).Length != 0)
                {
                    Field(field);
                }
            }
        }

        protected GUISkin customSkin;
        protected Color defaultColor;

        public void Toggle(SerializedProperty enableTrigger, string label)
        {
            GUILayout.BeginHorizontal(EditorStyles.helpBox);

            enableTrigger.boolValue = GUILayout.Toggle(enableTrigger.boolValue, new GUIContent(label), customSkin.FindStyle("Toggle"));
            enableTrigger.boolValue = GUILayout.Toggle(enableTrigger.boolValue, new GUIContent(""), customSkin.FindStyle("Toggle Helper"));

            GUILayout.EndHorizontal();
        }
        public void Toggle(string name)
        {
            var enableTrigger = serializedObject.FindProperty(name);
            GUILayout.BeginHorizontal(EditorStyles.helpBox);

            enableTrigger.boolValue = GUILayout.Toggle(enableTrigger.boolValue, new GUIContent(name), customSkin.FindStyle("Toggle"));
            enableTrigger.boolValue = GUILayout.Toggle(enableTrigger.boolValue, new GUIContent(""), customSkin.FindStyle("Toggle Helper"));

            GUILayout.EndHorizontal();
        }

        public override void OnInspectorGUI()
        {
            serializedObject.Update();

            defaultColor = GUI.color;

            if (EditorGUIUtility.isProSkin == true)
                customSkin = (GUISkin)Resources.Load("Editor\\ADUI Skin Dark");
            else
                customSkin = (GUISkin)Resources.Load("Editor\\ADUI Skin Light");

            GUILayout.BeginHorizontal();
            GUI.backgroundColor = defaultColor;

            GUILayout.Box(new GUIContent(""), customSkin.FindStyle(TopHeader));

            GUILayout.EndHorizontal();
            GUILayout.Space(-42);

            GUIContent[] toolbarTabs = new GUIContent[3];
            toolbarTabs[0] = new GUIContent("Content");
            toolbarTabs[1] = new GUIContent("Resources");
            toolbarTabs[2] = new GUIContent("Settings");

            GUILayout.BeginHorizontal();
            GUILayout.Space(17);

            currentTab = GUILayout.Toolbar(currentTab, toolbarTabs, customSkin.FindStyle("Tab Indicator"));

            GUILayout.EndHorizontal();
            GUILayout.Space(-40);
            GUILayout.BeginHorizontal();
            GUILayout.Space(17);

            if (GUILayout.Button(new GUIContent("Content", "Content"), customSkin.FindStyle("Tab Content")))
                currentTab = 0;
            if (GUILayout.Button(new GUIContent("Resources", "Resources"), customSkin.FindStyle("Tab Resources")))
                currentTab = 1;
            if (GUILayout.Button(new GUIContent("Settings", "Settings"), customSkin.FindStyle("Tab Settings")))
                currentTab = 2;

            GUILayout.EndHorizontal();

            switch (currentTab)
            {
                case 0:
                    {
                        HorizontalBlockWithBox(() => HelpBox("Content", MessageType.Info));
                        OnContentGUI();
                    }
                    break;

                case 1:
                    {
                        HorizontalBlockWithBox(() => HelpBox("Resources", MessageType.Info));
                        OnResourcesGUI();
                    }
                    break;

                case 2:
                    {
                        HorizontalBlockWithBox(() => HelpBox("Setting", MessageType.Info));
                        OnSettingsGUI();
                    }
                    break;
            }

            serializedObject.ApplyModifiedProperties();
        }

        public void OnDefaultInspectorGUI()
        {
            base.OnInspectorGUI();
        }

        public void MakeUpNumericManager(string thatNumericManagerName)
        {
            SerializedProperty property = serializedObject.FindProperty(thatNumericManagerName);
            VerticalBlockWithBox(() =>
            {
                if (property.stringValue.StartsWith("Default"))
                    HelpBox("Numeric Manager Is Idle", MessageType.Info);
                else
                    HelpBox("You Can Name It Start With Default To Make It Idle", MessageType.Info);
                EditorGUILayout.PropertyField(property);
            });
        }
    }

    [CustomEditor(typeof(MonoAnyBehaviour), true)]
    public class AnyBehaviourEditor : AbstractCustomEditor { }

    public abstract class EditorWindow : UnityEditor.EditorWindow
    {
        /*
         [MenuItem("MyWindow/Window")]
        static void window()
        {
            Mybianyi mybianyi = GetWindow<Mybianyi>();
            mybianyi.Show();
        }
        private void OnGUI()
         */
    }
}
