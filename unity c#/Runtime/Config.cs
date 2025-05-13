using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Threading;
using Convention.Internal;
using UnityEditor;
using UnityEngine;
using UnityEngine.Events;

#if UNITY_2017_1_OR_NEWER
namespace UnityEditor
{

}
#endif

namespace Convention
{
    public static partial class ConventionUtility
    {
        public static object GetDefault([In] Type type)
        {
            if (type.IsClass)
                return null;
            else
                return System.Activator.CreateInstance(type);
        }
    }

    namespace Internal
    {
        public class Indicator
        {
            public static Type tag { get; protected set; }
            public static bool value { get; protected set; }
        }
        public interface ConventionEditorInspectorGUI { }
    }

    public class PlatformIndicator : Indicator
    {
        static PlatformIndicator()
        {
            Indicator.tag = typeof(void);
#if DEBUG
            Indicator.value = false;
#else
            Indicator.value = true;
#endif
        }
        public static bool is_release => Indicator.value;
        public static bool is_platform_windows => RuntimeInformation.IsOSPlatform(OSPlatform.Windows);
        public static bool is_platform_linux => RuntimeInformation.IsOSPlatform(OSPlatform.Linux);
        public static bool is_platform_osx => RuntimeInformation.IsOSPlatform(OSPlatform.OSX);
        public static bool is_platform_x64 => System.Environment.Is64BitOperatingSystem;
    }

    public static partial class ConventionUtility
    {
        //[MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static string convert_xstring([In] object obj)
        {
            return Convert.ToString(obj);
        }
        public static _T convert_xvalue<_T>([In] string str)
        {
            Type type = typeof(_T);
            var parse_method = type.GetMethod("Parse");
            if (parse_method != null &&
                (parse_method.ReturnType.IsSubclassOf(type) || parse_method.ReturnType == type) &&
                parse_method.GetParameters().Length == 1 &&
                parse_method.GetParameters()[0].ParameterType == typeof(string))
            {
                return (_T)parse_method.Invoke(null, new object[] { str });
            }

            throw new InvalidCastException($"\"{str}\" is cannt convert to type<{type}>");
        }
        public static string Combine([In] params object[] args)
        {
            if (args.Length == 0)
                return "";
            if (args.Length == 1)
                return args[0].ToString();
            return Combine(args[0]) + Combine(args[1..]);
        }
        public static string Trim([In] string str, int left_right_flag = 3)
        {
            string result = new string(str);
            if ((left_right_flag & (1 << 0)) == 1)
                result = result.TrimStart();
            if ((left_right_flag & (1 << 1)) == 1)
                result = result.TrimEnd();
            return result;
        }

        public static object SeekValue([In] object obj, [In] string name, BindingFlags flags, [Out][Opt] out bool isSucceed)
        {
            Type type = obj.GetType();
            var field = type.GetField(name, flags);
            isSucceed = true;
            if (field != null)
            {
                return field.GetValue(obj);
            }
            var property = type.GetProperty(name, flags);
            if (property != null)
            {
                return property.GetValue(obj);
            }
            isSucceed = false;
            return null;
        }
        public static object SeekValue([In] object obj, [In] string name, BindingFlags flags)
        {
            Type type = obj.GetType();
            var field = type.GetField(name, flags);
            if (field != null)
            {
                return field.GetValue(obj);
            }
            var property = type.GetProperty(name, flags);
            if (property != null)
            {
                return property.GetValue(obj);
            }
            return null;
        }
        public static object SeekValue([In] object obj, [In] string name, [In] Type valueType, BindingFlags flags, [Out][Opt] out bool isSucceed)
        {
            Type type = obj.GetType();
            var field = type.GetField(name, flags);
            isSucceed = true;
            if (field != null && field.FieldType == valueType)
            {
                return field.GetValue(obj);
            }
            var property = type.GetProperty(name, flags);
            if (property != null && property.PropertyType == valueType)
            {
                return property.GetValue(obj);
            }
            isSucceed = false;
            return null;
        }
        public static object SeekValue([In] object obj, [In] string name, [In] Type valueType, BindingFlags flags)
        {
            Type type = obj.GetType();
            var field = type.GetField(name, flags);
            if (field != null && field.FieldType == valueType)
            {
                return field.GetValue(obj);
            }
            var property = type.GetProperty(name, flags);
            if (property != null && property.PropertyType == valueType)
            {
                return property.GetValue(obj);
            }
            return null;
        }
        public static bool PushValue([In] object obj, [In] object value, [In] string name, BindingFlags flags)
        {
            Type type = obj.GetType();
            var field = type.GetField(name, flags);
            if (field != null)
            {
                field.SetValue(obj, value);
                return true;
            }
            var property = type.GetProperty(name, flags);
            if (property != null)
            {
                property.SetValue(obj, value);
                return true;
            }
            return false;
        }

        public static T GetOrAddComponent<T>(this MonoAnyBehaviour self) where T : Component
        {
            if (self.GetComponents<T>().Length == 0)
            {
                return self.gameObject.AddComponent<T>();
            }
            else
            {
                return self.gameObject.GetComponents<T>()[0];
            }
        }
        public static T SeekComponent<T>(this MonoAnyBehaviour self) where T : class
        {
            var results = self.gameObject.GetComponents<T>();
            if (results.Length == 0)
                return null;
            return results[0];
        }
        public static T GetOrAddComponent<T>(this GameObject self) where T : Component
        {
            if (self.GetComponents<T>().Length == 0)
            {
                return self.AddComponent<T>();
            }
            else
            {
                return self.GetComponents<T>()[0];
            }
        }
        public static T SeekComponent<T>(this GameObject self) where T : class
        {
            var results = self.GetComponents<T>();
            if (results.Length == 0)
                return null;
            return results[0];
        }

        public static List<Type> SeekType(Predicate<Type> pr, IEnumerable<Assembly> assemblys = null, int findCount = -1)
        {
            List<Type> types = new List<Type>();
            if (assemblys == null)
                assemblys = AppDomain.CurrentDomain.GetAssemblies();
            foreach (var assembly in assemblys)
            {
                foreach (var type in assembly.GetTypes())
                {
                    if (pr(type))
                        types.Add(type);
                    if (types.Count == findCount)
                        return types;
                }
            }
            return types;
        }

        public static List<MemberInfo> GetMemberInfos(Type type, IEnumerable<Type> cutOffType = null, bool isGetNotPublic = false, bool isGetStatic = false)
        {
            Type current = type;
            BindingFlags flags = BindingFlags.Public | BindingFlags.Instance | BindingFlags.DeclaredOnly;
            List<MemberInfo> result = new();
            if (isGetNotPublic)
                flags |= BindingFlags.NonPublic;
            if (isGetStatic)
                flags |= BindingFlags.Static;
            while ((cutOffType != null && !cutOffType.Contains(current)) && current != null)
            {
                result.AddRange(current.GetFields(flags));
                result.AddRange(current.GetProperties(flags));
                result.AddRange(current.GetMethods(flags));
                current = current.BaseType;
            }
            return result;
        }
    }

    [Serializable]
    public class SALCheckException : Exception
    {
        public delegate bool Predicate(object val);
        public Attribute attribute;
        public SALCheckException(Attribute attribute) { this.attribute = attribute; }
        public SALCheckException(Attribute attribute, string message) : base(message) { this.attribute = attribute; }
        public SALCheckException(Attribute attribute, string message, Exception inner) : base(message, inner) { this.attribute = attribute; }
    }
    [System.AttributeUsage(AttributeTargets.Parameter, Inherited = true, AllowMultiple = false)]
    public class InAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.Parameter, Inherited = true, AllowMultiple = false)]
    public class OutAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.Parameter, Inherited = true, AllowMultiple = true)]
    public class OptAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.ReturnValue, Inherited = true, AllowMultiple = false)]
    public class ReturnVirtualAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.ReturnValue, Inherited = true, AllowMultiple = false)]
    public class ReturnMayNullAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.ReturnValue, Inherited = true, AllowMultiple = false)]
    public class ReturnNotNullAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.ReturnValue, Inherited = true, AllowMultiple = false)]
    public class ReturnSelfAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.ReturnValue, Inherited = true, AllowMultiple = false)]
    public class ReturnNotSelfAttribute : Attribute { }
#if UNITY_2017_1_OR_NEWER
    [System.AttributeUsage(AttributeTargets.Parameter | AttributeTargets.ReturnValue |
        AttributeTargets.Field | AttributeTargets.Property, Inherited = true, AllowMultiple = false)]
    public class IsInstantiatedAttribute : Attribute
    {
        public bool isInstantiated;
        public IsInstantiatedAttribute(bool isInstantiated)
        {
            this.isInstantiated = isInstantiated;
        }
    }
#endif
    [System.AttributeUsage(AttributeTargets.ReturnValue, Inherited = true, AllowMultiple = false)]
    public class SucceedAttribute : Attribute
    {
        private SALCheckException.Predicate pr;
        public SucceedAttribute([In] object succeed_when_return_value_is_equal_this_value_or_pr_is_return_true)
        {
            var prm = succeed_when_return_value_is_equal_this_value_or_pr_is_return_true.GetType().GetMethod("Invoke");
            if (prm != null &&
                prm.GetParameters().Length == 1 &&
                prm.ReturnType == typeof(bool))
                this.pr = (SALCheckException.Predicate)succeed_when_return_value_is_equal_this_value_or_pr_is_return_true;
            else
                this.pr = (object obj) => obj == succeed_when_return_value_is_equal_this_value_or_pr_is_return_true;
        }

        public bool Check([In][Opt] object value)
        {
            if (this.pr(value))
                return true;
            throw new SALCheckException(this, $"return value<{value.ToString()[..25]}...> is not expect");
        }
    }
    [System.AttributeUsage(AttributeTargets.ReturnValue, Inherited = true, AllowMultiple = false)]
    public class NotSucceedAttribute : Attribute
    {
        private SALCheckException.Predicate pr;
        public NotSucceedAttribute([In] object failed_when_return_value_is_equal_this_value_or_pr_is_return_true)
        {
            var prm = failed_when_return_value_is_equal_this_value_or_pr_is_return_true.GetType().GetMethod("Invoke");
            if (prm != null &&
                prm.GetParameters().Length == 1 &&
                prm.ReturnType == typeof(bool))
                this.pr = (SALCheckException.Predicate)failed_when_return_value_is_equal_this_value_or_pr_is_return_true;
            else
                this.pr = (object obj) => obj == failed_when_return_value_is_equal_this_value_or_pr_is_return_true;
        }

        public bool Check([In][Opt] object value)
        {
            if (this.pr(value))
                throw new SALCheckException(this, $"return value<{value.ToString()[..25]}...> is not expect");
            return true;
        }
    }
    [System.AttributeUsage(AttributeTargets.Method, Inherited = true, AllowMultiple = false)]
    public class MethodReturnSelfAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.Method, Inherited = true, AllowMultiple = false)]
    public class MethodReturnNotSelfAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.All, Inherited = true, AllowMultiple = true)]
    public class WhenAttribute : Attribute
    {
        private SALCheckException.Predicate pr = null;
        public readonly Type TypenAttribute = null;
        /// <summary>
        /// <list type="bullet">bool predicate(object)+typenAttribute</list>
        /// <list type="bullet">value+typenAttribute</list>
        /// </summary>
        /// <param name="control_value_or_predicate">The value will been checked or predicate</param>
        /// <param name="typenAttribute">Target Checker</param>
        public WhenAttribute([In] object control_value_or_predicate, [In] Type typenAttribute)
        {
            this.TypenAttribute = typenAttribute;
            if (typenAttribute == typeof(OnlyNotNullModeAttribute))
            {
                if (ConventionUtility.IsString(control_value_or_predicate))
                {
                    this.pr = (object obj) =>
                    {
                        return new OnlyNotNullModeAttribute((string)control_value_or_predicate).Check(obj);
                    };
                    return;
                }
            }
            else if (
                typenAttribute.Name.EndsWith("SucceedAttribute") ||
                typenAttribute.Name.StartsWith("Return")
                )
            {
                return;
            }
            do
            {
                var prm = control_value_or_predicate.GetType().GetMethod("Invoke");
                if (prm != null &&
                    prm.GetParameters().Length == 1 &&
                    prm.ReturnType == typeof(bool))
                    this.pr = (SALCheckException.Predicate)control_value_or_predicate;
                else
                    this.pr = (object obj) => obj == control_value_or_predicate;
            } while (false);
        }
        /// <summary>
        /// do nothing
        /// </summary>
        /// <param name="description"></param>
        public WhenAttribute([In] string description) { }
        protected WhenAttribute() { }

#if UNITY_EDITOR
        /// <summary>
        /// The value is <see cref="UnityEditor.Editor.target"/>
        /// <list type="bullet"><b><see cref="TypenAttribute"/> is <see cref="OnlyNotNullModeAttribute"/>:</b>member value is not null</list>
        /// <list type="bullet"><b>Default:</b>predicate(target)</list>
        /// </summary>
        /// <param name="value"><see cref="UnityEditor.Editor.target"/></param>
        /// <returns></returns>
#endif
        public virtual bool Check([In][Opt] object value)
        {
            if (TypenAttribute == typeof(OnlyNotNullModeAttribute))
            {
                return pr(value);
            }
            else
            {
                if (pr == null)
                    //throw new SALCheckException(this, "you should not check at this");
                    return true;
                if (this.pr(value))
                    return true;
                return false;
            }
        }

        [System.AttributeUsage(AttributeTargets.Field | AttributeTargets.Property, Inherited = true, AllowMultiple = true)]
        public abstract class WhenMemberValueAttribute : WhenAttribute
        {
            public readonly string Name;
            public readonly object Value;
            protected object InjectGetValue(object target)
            {
                //var field = target.GetType().GetField(Name, BindingFlags.NonPublic | BindingFlags.Public |
                //    BindingFlags.Instance | BindingFlags.Static);
                //if (field != null)
                //{
                //    return field.GetValue(target);
                //}
                //var property = target.GetType().GetProperty(Name, BindingFlags.NonPublic | BindingFlags.Public |
                //    BindingFlags.Instance | BindingFlags.Static);
                //if (property != null)
                //{
                //    return property.GetValue(target);
                //}
                //return null;
                return ConventionUtility.SeekValue(target, Name, BindingFlags.NonPublic | BindingFlags.Public |
                   BindingFlags.Instance | BindingFlags.Static);
            }
            public override bool Check(object target)
            {
                throw new NotImplementedException();
            }
            public WhenMemberValueAttribute(string Name, object value)
            {
                this.Name = Name;
                this.Value = value;
            }
        }
        [System.AttributeUsage(AttributeTargets.Field | AttributeTargets.Property, Inherited = true, AllowMultiple = true)]
        public class IsAttribute : WhenMemberValueAttribute
        {
            public override bool Check(object target)
            {
                if (this.Value is Type)
                {
                    var targetValue = this.InjectGetValue(target);
                    if (targetValue == null)
                        return false;
                    else
                        return targetValue.GetType().IsSubclassOf(this.Value as Type);
                }
                if (this.Value != null)
                    return this.Value.Equals(this.InjectGetValue(target));
                var injectValue = this.InjectGetValue(target);
                if (injectValue != null)
                    return injectValue.Equals(this.Value);
                return injectValue == null && this.Value == null;
            }
            public IsAttribute(string Name, object value) : base(Name, value) { }
        }
        [System.AttributeUsage(AttributeTargets.Field | AttributeTargets.Property, Inherited = true, AllowMultiple = true)]
        public class NotAttribute : IsAttribute
        {
            public override bool Check(object target)
            {
                return !base.Check(target);
            }
            public NotAttribute(string Name, object value) : base(Name, value) { }
        }
    }
    [System.AttributeUsage(AttributeTargets.All, Inherited = false, AllowMultiple = false)]
    public class IgnoreAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.All, Inherited = false, AllowMultiple = false)]
    public class ProjectContextLabelAttribute : Attribute
    {
        public enum ContextLabelType
        {
            Content, Resources, Setting
        }
        public static void DebugError(string mainName, string message, ContextLabelType type, UnityEngine.Object obj)
        {
            string labelname = type switch
            {
                ContextLabelType.Setting => "setting",
                ContextLabelType.Resources => "resources",
                ContextLabelType.Content => "content",
                _ => "context-label"
            };
            Debug.LogError($"{mainName} - {message} due to missing {labelname}.", obj);
        }
    }
    [System.AttributeUsage(AttributeTargets.All, Inherited = false, AllowMultiple = false)]
    public class SettingAttribute : ProjectContextLabelAttribute { }
    [System.AttributeUsage(AttributeTargets.All, Inherited = false, AllowMultiple = false)]
    public class ResourcesAttribute : ProjectContextLabelAttribute { }
    [System.AttributeUsage(AttributeTargets.All, Inherited = false, AllowMultiple = false)]
    public class ContentAttribute : ProjectContextLabelAttribute { }
    [System.AttributeUsage(AttributeTargets.All, Inherited = false, AllowMultiple = false)]
    public class OnlyPlayModeAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.All, Inherited = false, AllowMultiple = false)]
    public class OnlyNotNullModeAttribute : Attribute
    {
        public string Name;
        public bool Check(object target)
        {
            if (IsSelf())
            {
#if UNITY_2017_1_OR_NEWER
                if (target is UnityEngine.Object && (target as UnityEngine.Object) == null)
                    return false;
#endif
                return target != null;
            }
            var field = target.GetType().GetField(Name, BindingFlags.Static | BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
            if (field != null)
            {
                object value = field.GetValue(target);
                if (value == null)
                    return false;
#if UNITY_2017_1_OR_NEWER
                if (value is UnityEngine.Object && (value as UnityEngine.Object) == null)
                    return false;
#endif
                return true;
            }
            var property = target.GetType().GetProperty(Name, BindingFlags.Static | BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
            if (property != null)
            {
                object value = property.GetValue(target);
                if (value == null)
                    return false;
#if UNITY_2017_1_OR_NEWER
                if (value is UnityEngine.Object && (value as UnityEngine.Object) == null)
                    return false;
#endif
                return true;
            }
            return false;
        }
        public bool IsSelf() => Name == null || Name.Length == 0;
        /// <summary>
        /// binding to target field
        /// </summary>
        /// <param name="fieldName"></param>
        public OnlyNotNullModeAttribute(string fieldName) { this.Name = fieldName; }
        /// <summary>
        /// binding to self
        /// </summary>
        public OnlyNotNullModeAttribute() { this.Name = null; }
    }
    [System.AttributeUsage(AttributeTargets.All, Inherited = false, AllowMultiple = false)]
    public class HopeNotNullAttribute : Attribute
    {
        public bool Check(object target)
        {
            return target != null;
        }
        public HopeNotNullAttribute() { }
    }
    [System.AttributeUsage(AttributeTargets.Field | AttributeTargets.Property |
        AttributeTargets.Parameter | AttributeTargets.ReturnValue,
        Inherited = false, AllowMultiple = false)]
    public class PercentageAttribute : Attribute
    {
        public float min = 0, max = 100;
        public PercentageAttribute([In] float min, [In] float max)
        {
            this.min = min;
            this.max = max;
        }
    }
    [System.AttributeUsage(AttributeTargets.Field | AttributeTargets.Property |
        AttributeTargets.Class | AttributeTargets.Class |
        AttributeTargets.Parameter | AttributeTargets.ReturnValue |
        AttributeTargets.Interface | AttributeTargets.GenericParameter, Inherited = false, AllowMultiple = false)]
    public class ArgPackageAttribute : Attribute
    {
        public Type[] UsedFor;
        public ArgPackageAttribute([In][Opt] params Type[] usedFor)
        {
            UsedFor = usedFor;
        }
    }
    [System.AttributeUsage(AttributeTargets.All, Inherited = false, AllowMultiple = false)]
    public class TODOAttribute : Attribute
    {
        public bool Check(object any)
        {
            throw new InvalidOperationException("TODO");
            throw new NotImplementedException();
        }
    }
    [System.AttributeUsage(AttributeTargets.All, Inherited = true, AllowMultiple = false)]
    public class ImportantAttribute : Attribute
    {
        public string description;

        public ImportantAttribute(string description)
        {
            this.description = description;
        }
        public ImportantAttribute() { }
    }
    [System.AttributeUsage(AttributeTargets.All, Inherited = false, AllowMultiple = true)]
    public class DescriptionAttribute : Attribute
    {
        public string description;
        public DescriptionAttribute(string description)
        {
            this.description = description;
        }
    }
    [System.AttributeUsage(AttributeTargets.All, Inherited = true, AllowMultiple = true)]
    public class TypeCheckAttribute : Attribute
    {
        public readonly Type[] typens;
        public readonly string description;

        public TypeCheckAttribute(string description, params Type[] typens)
        {
            this.typens = typens;
            this.description = description;
        }

        public TypeCheckAttribute(params Type[] typens) : this("Type Check Failed: value is not sub class of " +
            $"{string.Join(",", typens.ToList().ConvertAll(x => x.Name))}" +
            ", current is ${type}", typens)
        {

        }

        public bool Check(object target)
        {
            return target != null &&
                typens.Any(x => target.GetType().IsSubclassOf(x) || x.IsInterface && target.GetType().GetInterface(x.Name) != null);
        }
    }


    public static partial class ConventionUtility
    {
#if UNITY_EDITOR
        [UnityEditor.MenuItem("Convention/InitExtensionEnv", priority = 100000)]
        public static void InitExtensionEnv()
        {
            UnityEngine.Application.quitting += () => CoroutineStarter = null;
            RegisterBaseWrapperExtension.InitExtensionEnv();
            SO.Windows.InitExtensionEnv();
            GlobalConfig.InitExtensionEnv();
            StringExtension.InitExtensionEnv();

            UnityExtension.InitExtensionEnv();

            ES3Plugin.InitExtensionEnv();
        }
        [UnityEditor.MenuItem("Convention/SetupURPDefined", priority = 1024)]
        public static void AddURPDefined()
        {
            ScriptingDefineUtility.Add("UNITY_URP", EditorUserBuildSettings.selectedBuildTargetGroup);
        }
        [UnityEditor.MenuItem("Convention/RemoveURPDefined", priority = 1024)]
        public static void RemoveURPDefined()
        {
            ScriptingDefineUtility.Add("UNITY_URP", EditorUserBuildSettings.selectedBuildTargetGroup);
        }
#endif

        public static int MainThreadID { get; private set; }
        public static bool CurrentThreadIsMainThread()
        {
            return MainThreadID == Thread.CurrentThread.ManagedThreadId;
        }

        private static CoroutineMonoStarterUtil CoroutineStarter;

        private class CoroutineMonoStarterUtil : MonoBehaviour
        {
            private void Update()
            {
                MainThreadID = Thread.CurrentThread.ManagedThreadId;
            }

            private void OnDestroy()
            {
                CoroutineStarter = null;
            }
        }
        public static Coroutine StartCoroutine(IEnumerator coroutine)
        {
            if (CoroutineStarter == null)
            {
                CoroutineStarter = new GameObject($"{nameof(ConventionUtility)}-{nameof(CoroutineStarter)}").AddComponent<CoroutineMonoStarterUtil>();
            }
            return CoroutineStarter.StartCoroutine(coroutine);
        }
        public static void CloseCoroutine(Coroutine coroutine)
        {
            CoroutineStarter.StopCoroutine(coroutine);
        }
        public static void StopAllCoroutine()
        {
            CoroutineStarter.StopAllCoroutines();
        }
        /// <summary>
        /// 不要保存这个类, 此类在析构后执行任务
        /// </summary>
        public class ActionStepCoroutineWrapper:AnyClass
        {
            private List<KeyValuePair<YieldInstruction, Action>> steps = new();
            public ActionStepCoroutineWrapper Update(Action action)
            {
                steps.Add(new(null, action));
                return this;
            }
            public ActionStepCoroutineWrapper Wait(float time,Action action)
            {
                steps.Add(new(new WaitForSeconds(time), action));
                return this;
            }
            public ActionStepCoroutineWrapper FixedUpdate(Action action)
            {
                steps.Add(new(new WaitForFixedUpdate(), action));
                return this;
            }
            public ActionStepCoroutineWrapper Next(Action action)
            {
                steps.Add(new(new WaitForEndOfFrame(), action));
                return this;
            }
            private static IEnumerator Execute(List<KeyValuePair<YieldInstruction, Action>> steps)
            {
                foreach (var (waiting,action) in steps)
                {
                    action();
                    yield return waiting;
                }
            }
            ~ActionStepCoroutineWrapper()
            {
                this.Invoke();
            }
            public void Invoke()
            {
                StartCoroutine(Execute(new List<KeyValuePair<YieldInstruction, Action>>(steps)));
                steps.Clear();
            }
        }
        /// <summary>
        /// 不要接收返回值, 否则将延迟到Wrapper被析构后才执行任务
        /// </summary>
        /// <returns></returns>
        [return: ReturnNotSelf]
        public static ActionStepCoroutineWrapper CreateSteps() => new();

        public static bool IsNumber([In] object data)
        {
            if (data == null) return false;
            var type = data.GetType();
            return IsNumber(type);
        }
        public static bool IsString([In] object data)
        {
            if (data == null) return false;
            var type = data.GetType();
            return IsString(type);
        }
        public static bool IsBinary([In] object data)
        {
            if (data == null) return false;
            var type = data.GetType();
            return IsBinary(type);
        }
        public static bool IsArray([In] object data)
        {
            if (data == null) return false;
            var type = data.GetType();
            return IsArray(type);
        }
        public static bool IsBool([In] object data)
        {
            if (data == null) return false;
            return IsBool(data.GetType());
        }

        public static bool IsNumber([In] Type type)
        {
            return
                type == typeof(double) ||
                type == typeof(float) ||
                type == typeof(int) ||
                type == typeof(long) ||
                type == typeof(sbyte) ||
                type == typeof(short) ||
                type == typeof(ushort) ||
                type == typeof(uint) ||
                type == typeof(ulong) ||
                type == typeof(char);
        }
        public static bool IsString([In] Type type)
        {
            return type == typeof(string) || type == typeof(char[]);
        }
        public static bool IsBinary([In] Type type)
        {
            return
                type == typeof(byte) ||
                type == typeof(sbyte) ||
                type == typeof(byte[]) ||
                type == typeof(sbyte[]);
        }
        public static bool IsArray([In] Type type)
        {
            return type.IsArray;
        }
        public static bool IsBool([In] Type type)
        {
            return type == typeof(bool);
        }
        public static bool IsEnum([In]Type type)
        {
            return type.IsEnum;
        }
        public static bool IsImage([In]Type type)
        {
            return type.IsSubclassOf(typeof(Texture)) || type == typeof(Sprite);
        }

        public static bool HasCustomAttribute([In] MemberInfo member, [In] IEnumerable<Type> attrs)
        {
            foreach (var attr in attrs)
            {
                if (member.GetCustomAttribute(attr, true) != null)
                    return true;
            }
            return false;
        }
        [return: ReturnMayNull]
        public static Type GetMemberValueType([In] MemberInfo member)
        {
            if (member is FieldInfo field)
            {
                return field.FieldType;
            }
            else if (member is PropertyInfo property)
            {
                return property.PropertyType;
            }
            return null;
        }
        public static bool GetMemberValueType([In] MemberInfo member, [Out] out Type type)
        {
            if (member is FieldInfo field)
            {
                type = field.FieldType;
                return true;
            }
            else if (member is PropertyInfo property)
            {
                type = property.PropertyType;
                return true;
            }
            type = null;
            return false;
        }
        public static void PushValue([In] object target, [In][Opt, When("If you sure")] object value, [In] MemberInfo info)
        {
            if (info is FieldInfo field)
            {
                if (value.GetType().IsSubclassOf(field.FieldType))
                    field.SetValue(target, value);
                else
                {
                    field.SetValue(target, field.FieldType.GetMethod(nameof(float.Parse)).Invoke(target, new object[] { value }));
                }
            }
            else if (info is PropertyInfo property)
            {
                property.SetValue(target, value);
            }
            else
            {
                throw new InvalidOperationException("info is unsupport");
            }
        }
        public static object SeekValue([In] object target, [In] MemberInfo info)
        {
            if (info is FieldInfo field)
            {
                return field.GetValue(target);
            }
            else if (info is PropertyInfo property)
            {
                return property.GetValue(target);
            }
            else
            {
                throw new InvalidOperationException("info is unsupport");
            }
        }
        public static bool TrySeekValue([In] object target, [In] MemberInfo info, [Out] out object value)
        {
            if (info is FieldInfo field)
            {
                value = field.GetValue(target);
                return true;
            }
            else if (info is PropertyInfo property)
            {
                value = property.GetValue(target);
                return true;
            }
            value = null;
            return false;
        }

        public static List<MemberInfo> SeekMemberInfo(
            [In] object target,
            [In, Opt] IEnumerable<Type> attrs, [In, Opt] IEnumerable<Type> types,
            [In, Opt] Type untilBase = null
            )
        {
            Type _CurType = target.GetType();
            List<MemberInfo> result = new();
            result.AddRange(_CurType.GetMembers(BindingFlags.Public | BindingFlags.Instance));
            while (_CurType != null && _CurType != typeof(object) && _CurType != untilBase)
            {
                result.AddRange(
                    from info in _CurType.GetMembers(BindingFlags.NonPublic | BindingFlags.Instance)
                    where attrs == null || HasCustomAttribute(info, attrs)
                    where types == null || (GetMemberValueType(info, out var type) && types.Contains(type))
                    select info
                    );
                _CurType = _CurType.BaseType;
            }
            return result;
        }
        public static List<MemberInfo> SeekMemberInfo([In] object target, IEnumerable<string> names, BindingFlags flags = BindingFlags.Default)
        {
            Type _CurType = target.GetType();
            List<MemberInfo> result = _CurType.GetMembers(flags).ToList();
            HashSet<string> nameSet = names.ToHashSet();
            result.RemoveAll(x => nameSet.Contains(x.Name) == false);
            return result;
        }
        public static object InvokeMember([In] MemberInfo member, [In] object target, params object[] parameters)
        {
            if (member is MethodInfo method)
            {
                return method.Invoke(target, parameters);
            }
            return null;
        }
        public static bool TryInvokeMember([In] MemberInfo member, object target, out object returnValue, params object[] parameters)
        {
            returnValue = null;
            if (member is MethodInfo method)
            {
                returnValue = method.Invoke(target, parameters);
                return true;
            }
            else return false;
        }
    }

#if UNITY_2017_1_OR_NEWER
    namespace Internal
    {
        public interface IRectTransform
        {
            UnityEngine.RectTransform rectTransform { get; }
        }
    }
#endif

    public class PerformanceIndicator : Indicator
    {
        [Flags, Serializable]
        public enum PerformanceMode
        {
            Speed = 0,
            Quality = (1 << 8) - 1,
            L1 = 1 << 0,
            L2 = 1 << 1,
            L3 = 1 << 2,
            L4 = 1 << 3,
            L5 = 1 << 4,
            L6 = 1 << 5,
            L7 = 1 << 6,
            L8 = 1 << 7,
        }
        private static PerformanceMode m_mode = PerformanceMode.Quality;
        public static PerformanceMode mode
        {
            get => m_mode;
            set
            {
                PerformanceIndicator.value = value == PerformanceMode.Quality;
            }
        }
    }

    public static partial class ConventionUtility
    {
        public static UnityEvent WrapperAction2Event(params UnityAction[] actions)
        {
            var result = new UnityEvent();
            foreach (var action in actions)
            {
                result.AddListener(action);
            }
            return result;
        }
        public static UnityEvent<T> WrapperAction2Event<T>(params UnityAction<T>[] actions)
        {
            var result = new UnityEvent<T>();
            foreach (var action in actions)
            {
                result.AddListener(action);
            }
            return result;
        }
        public static UnityEvent<T, Y> WrapperAction2Event<T, Y>(params UnityAction<T, Y>[] actions)
        {
            var result = new UnityEvent<T, Y>();
            foreach (var action in actions)
            {
                result.AddListener(action);
            }
            return result;
        }
        public static UnityEvent<T, Y, U> WrapperAction2Event<T, Y, U>(params UnityAction<T, Y, U>[] actions)
        {
            var result = new UnityEvent<T, Y, U>();
            foreach (var action in actions)
            {
                result.AddListener(action);
            }
            return result;
        }
        public static UnityEvent<T, Y, U, I> WrapperAction2Event<T, Y, U, I>(params UnityAction<T, Y, U, I>[] actions)
        {
            var result = new UnityEvent<T, Y, U, I>();
            foreach (var action in actions)
            {
                result.AddListener(action);
            }
            return result;
        }
    }
}
