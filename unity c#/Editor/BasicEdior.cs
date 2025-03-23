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
    [CustomEditor(typeof(ConventionEditorInspectorGUI), true, isFallback = true)]
    public class AbstractCustomEditor : Editor
    {
        protected int currentTab = 0;
        protected IEnumerable<FieldInfo> ContentFields;
        protected IEnumerable<MethodInfo> ContentMethods;
        protected IEnumerable<FieldInfo> ResourcesFields;
        protected IEnumerable<MethodInfo> ResourcesMethods;
        protected IEnumerable<FieldInfo> SettingFields;
        protected IEnumerable<MethodInfo> SettingMethods;

        protected virtual string TopHeader => "CM Top Header";

        protected void OnEnable()
        {
            Type _CurType = target.GetType();
            List<FieldInfo> fields = new();
            List<MethodInfo> methods = new();
            fields.AddRange(_CurType.GetFields(BindingFlags.Public |
                BindingFlags.Instance | BindingFlags.Static));
            methods.AddRange(_CurType.GetMethods(BindingFlags.Public |
                BindingFlags.Instance | BindingFlags.Static));
            while (_CurType != null && _CurType != typeof(UnityEngine.MonoBehaviour) && _CurType != typeof(object))
            {
                fields.AddRange(_CurType.GetFields(BindingFlags.NonPublic |
                    BindingFlags.Instance | BindingFlags.Static));
                methods.AddRange(_CurType.GetMethods(BindingFlags.NonPublic |
                    BindingFlags.Instance | BindingFlags.Static));
                _CurType = _CurType.BaseType;
            }
            fields.RemoveAll((field) =>
            {
                bool NotContent = field.GetCustomAttributes(typeof(ContentAttribute), true).Length == 0;
                bool NotResources = field.GetCustomAttributes(typeof(ResourcesAttribute), true).Length == 0;
                bool NotSetting = field.GetCustomAttributes(typeof(SettingAttribute), true).Length == 0;
                return field.IsPrivate && NotContent && NotResources && NotSetting;
            });
            static bool ContentCheck(FieldInfo field)
            {
                bool isContent = field.GetCustomAttributes(typeof(ContentAttribute), true).Length != 0;
                bool isResources = field.GetCustomAttributes(typeof(ResourcesAttribute), true).Length != 0;
                bool isSetting = field.GetCustomAttributes(typeof(SettingAttribute), true).Length != 0;
                return
                    isContent ||
                    (!isResources && !isSetting && !field.FieldType.IsSubclassOf(typeof(UnityEngine.Object)));
            }
            ContentFields = from field in fields
                            where ContentCheck(field)
                            select field;
            ContentMethods = from method in methods
                             where method.GetCustomAttributes(typeof(ContentAttribute), true).Length != 0
                             //where method.GetParameters().Length == 0
                             select method;
            static bool ResourcesCheck(FieldInfo field)
            {
                bool isContent = field.GetCustomAttributes(typeof(ContentAttribute), true).Length != 0;
                bool isResources = field.GetCustomAttributes(typeof(ResourcesAttribute), true).Length != 0;
                bool isSetting = field.GetCustomAttributes(typeof(SettingAttribute), true).Length != 0;
                return
                    isResources ||
                    (!isContent && !isSetting && field.FieldType.IsSubclassOf(typeof(UnityEngine.Object)));
            }
            ResourcesFields = from field in fields
                              where ResourcesCheck(field)
                              select field;
            ResourcesMethods = from method in methods
                               where method.GetCustomAttributes(typeof(ResourcesAttribute), true).Length != 0
                               //where method.GetParameters().Length == 0
                               select method;
            static bool SettingCheck(FieldInfo field)
            {
                return field.GetCustomAttributes(typeof(SettingAttribute), true).Length != 0;
            }
            SettingFields = from field in fields
                            where SettingCheck(field)
                            select field;
            SettingMethods = from method in methods
                             where method.GetCustomAttributes(typeof(SettingAttribute), true).Length != 0
                             //where method.GetParameters().Length == 0
                             select method;
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
            bool HasOnlyPlayMode = field.GetCustomAttributes(typeof(OnlyPlayModeAttribute), true).Length != 0;
            bool HasWhen = field.GetCustomAttributes(typeof(WhenAttribute), true).Length != 0;
            bool HasOnlyNotNullMode = field.GetCustomAttributes(typeof(OnlyNotNullModeAttribute), true).Length != 0;
            bool HasHopeNotNullMode = field.GetCustomAttributes(typeof(HopeNotNullAttribute), true).Length != 0;
            bool HasIgnore = field.GetCustomAttributes(typeof(IgnoreAttribute), true).Length != 0;
            bool HasSerializeField = field.GetCustomAttributes(typeof(SerializeField), true).Length != 0;
            bool HasOpt = field.GetCustomAttributes(typeof(OptAttribute), true).Length != 0;
            bool HasArgPackage = field.GetCustomAttributes(typeof(ArgPackageAttribute), true).Length != 0;

            if (HasOnlyPlayMode && Application.isPlaying == false)
            {
                OnlyDisplayOnPlayMode(field, isCheckIgnore);
                return;
            }
            else if (HasWhen)
            {
                foreach (var attr in field.GetCustomAttributes<WhenAttribute>(true))
                {
                    if (attr.Check(target) == false)
                        return;
                }
            }
            if (HasOnlyNotNullMode || HasHopeNotNullMode)
                DisplayOnlyNotNull(field, isCheckIgnore);
            else if (isCheckIgnore && (HasIgnore || (field.IsPublic == false && !HasSerializeField)))
                IgnoreField(field);
            else
                DisplayDefaultField(field, isCheckIgnore);

            void OnlyDisplayOnPlayMode(FieldInfo field, bool isCheckIgnore)
            {
                PlayModeField(field);
            }

            void DisplayBoolValue(FieldInfo field, bool isCheckIgnore)
            {
                if (isCheckIgnore)
                    this.Toggle(field.Name);
                else
                    field.SetValue(target, this.Toggle((bool)field.GetValue(target), field.Name));
            }

            void DisplayDefaultField(FieldInfo field, bool isCheckIgnore)
            {
                if (field.FieldType == typeof(bool))
                    DisplayBoolValue(field, isCheckIgnore);
                else
                {
                    var p = serializedObject.FindProperty(field.Name);
                    var tfattr = field.GetCustomAttribute<ToolFile.FileAttribute>(true);
                    if (tfattr != null)
                        GUILayout.BeginVertical(EditorStyles.helpBox);
                    if (p == null)
                    {
                        var parser = field.FieldType.GetMethod("Parse", new Type[] { typeof(string) });
                        if (parser != null)
                        {
                            GUILayout.BeginHorizontal();
                            GUILayout.Label(field.Name);
                            EditorGUI.BeginChangeCheck();
                            string str = GUILayout.TextField(field.GetValue(target).ToString());
                            if (EditorGUI.EndChangeCheck())
                            {
                                field.SetValue(target, parser.Invoke(null, new object[] { str }));
                            }
                            GUILayout.EndHorizontal();
                        }
                        else if (field.FieldType.FullName.StartsWith("System.") == false && isCheckIgnore)
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

            void DisplayOnlyNotNull(FieldInfo field, bool isCheckIgnore)
            {
                bool isWarning = false;
                bool isError = false;
                bool isNotDisplay = true;

                foreach (var attr in field.GetCustomAttributes<OnlyNotNullModeAttribute>(true))
                {
                    if (attr.IsSelf())
                    {
                        isNotDisplay = false;
                        if (!attr.Check(field.GetValue(target)))
                        {
                            isError = true;
                            break;
                        }
                    }
                    else
                    {
                        isNotDisplay = isNotDisplay && !attr.Check(target);
                    }
                }
                if (isError == false)
                {
                    foreach (var attr in field.GetCustomAttributes<HopeNotNullAttribute>(true))
                    {
                        isNotDisplay = false;
                        if (!attr.Check(field.GetValue(target)))
                        {
                            isWarning = true;
                            break;
                        }
                    }
                }
                if (isNotDisplay)
                    return;
                else if (isError)
                    VerticalBlockWithBox(() =>
                    {
                        HelpBox($"{field.Name} is null", MessageType.Error);
                        DisplayDefaultField(field, isCheckIgnore);
                    });
                else if (isWarning)
                    VerticalBlockWithBox(() =>
                    {
                        HelpBox($"{field.Name} is null", MessageType.Warning);
                        DisplayDefaultField(field, isCheckIgnore);
                    });
                else
                    DisplayDefaultField(field, isCheckIgnore);
            }
        }

        protected virtual void Method(MethodInfo method)
        {
            if (method.GetCustomAttributes(typeof(OnlyPlayModeAttribute), true).Length == 0 || Application.isPlaying)
                if (GUILayout.Button(method.Name))
                    method.Invoke(target, new object[0]);
        }

        public virtual void OnOriginGUI()
        {
            DrawDefaultInspector();
        }
        public virtual void OnContentGUI()
        {
            foreach (var method in ContentMethods)
            {
                if (method.IsStatic)
                {
                    Method(method);
                }
            }
            foreach (var field in ContentFields)
            {
                Field(field);
            }
            foreach (var method in ContentMethods)
            {
                if (!method.IsStatic)
                {
                    Method(method);
                }
            }
        }
        public virtual void OnResourcesGUI()
        {
            foreach (var method in ResourcesMethods)
            {
                if (method.IsStatic)
                {
                    Method(method);
                }
            }
            foreach (var field in ResourcesFields)
            {
                Field(field);
            }
            foreach (var method in ResourcesMethods)
            {
                if (!method.IsStatic)
                {
                    Method(method);
                }
            }
        }
        public virtual void OnSettingsGUI()
        {
            foreach (var method in SettingMethods)
            {
                if (method.IsStatic)
                {
                    Method(method);
                }
            }
            foreach (var field in SettingFields)
            {
                Field(field);
            }
            foreach (var method in SettingMethods)
            {
                if (!method.IsStatic)
                {
                    Method(method);
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
            if (name.StartsWith("m_"))
            {
                name = name[2..];
            }
            Toggle(enableTrigger, name);
        }
        public bool Toggle(bool value, string label)
        {
            bool result;
            GUILayout.BeginHorizontal(EditorStyles.helpBox);

            result = GUILayout.Toggle(value, new GUIContent(label), customSkin.FindStyle("Toggle"));
            result = GUILayout.Toggle(value, new GUIContent(""), customSkin.FindStyle("Toggle Helper"));

            GUILayout.EndHorizontal();
            return result;
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

            bool ContentNotEmpty = ContentFields.Count() != 0;
            bool ResourcesNotEmpty = ResourcesFields.Count() != 0;
            bool SettingNotEmpty = SettingFields.Count() != 0;
            List<GUIContent> toolbarTabs = new();
            if ((ContentNotEmpty ? 1 : 0) + (ResourcesNotEmpty ? 1 : 0) + (SettingNotEmpty ? 1 : 0) != 1)
                toolbarTabs.Add(new GUIContent("Origin"));
            if (ContentNotEmpty)
                toolbarTabs.Add(new GUIContent("Content"));
            if (ResourcesFields.Count() != 0)
                toolbarTabs.Add(new GUIContent("Resources"));
            if (SettingFields.Count() != 0)
                toolbarTabs.Add(new GUIContent("Settings"));

            GUILayout.BeginHorizontal();
            GUILayout.Space(17);

            currentTab = GUILayout.Toolbar(currentTab, toolbarTabs.ToArray(), customSkin.FindStyle("Tab Indicator"));

            GUILayout.EndHorizontal();
            GUILayout.Space(-40);
            GUILayout.BeginHorizontal();
            GUILayout.Space(17);

            if ((ContentNotEmpty ? 1 : 0) + (ResourcesNotEmpty ? 1 : 0) + (SettingNotEmpty ? 1 : 0) != 1)
                GUILayout.Button(new GUIContent("Origin", "Origin"), customSkin.FindStyle("Tab Data"));
            if (ContentFields.Count() != 0)
                GUILayout.Button(new GUIContent("Content", "Content"), customSkin.FindStyle("Tab Content"));
            if (ResourcesFields.Count() != 0)
                GUILayout.Button(new GUIContent("Resources", "Resources"), customSkin.FindStyle("Tab Resources"));
            if (SettingFields.Count() != 0)
                GUILayout.Button(new GUIContent("Settings", "Settings"), customSkin.FindStyle("Tab Settings"));

            GUILayout.EndHorizontal();

            string currentTabStr = toolbarTabs[currentTab].text;
            if (currentTabStr == "Content")
            {
                HorizontalBlockWithBox(() => HelpBox("Content", MessageType.Info));
                OnContentGUI();
            }
            else if (currentTabStr == "Resources")
            {
                HorizontalBlockWithBox(() => HelpBox("Resources", MessageType.Info));
                OnResourcesGUI();
            }
            else if (currentTabStr == "Settings")
            {
                HorizontalBlockWithBox(() => HelpBox("Settings", MessageType.Info));
                OnSettingsGUI();
            }
            else
            {
                HorizontalBlockWithBox(() => HelpBox("Origin", MessageType.Info));
                OnOriginGUI();
            }

            serializedObject.ApplyModifiedProperties();
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
