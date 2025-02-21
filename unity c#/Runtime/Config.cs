using System;
using System.Reflection;
using System.Runtime.InteropServices;
using Convention.Internal;

#if UNITY_2017_1_OR_NEWER
namespace UnityEditor
{

}
#endif

namespace Convention
{
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
    [System.AttributeUsage(AttributeTargets.Parameter, Inherited = true, AllowMultiple = false)]
    public class OptAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.ReturnValue, Inherited = true, AllowMultiple = false)]
    public class ReturnMayNullAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.ReturnValue, Inherited = true, AllowMultiple = false)]
    public class ReturnNotNullAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.ReturnValue, Inherited = true, AllowMultiple = false)]
    public class ReturnSelfAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.ReturnValue, Inherited = true, AllowMultiple = false)]
    public class ReturnNotSelfAttribute : Attribute { }
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
                var field = target.GetType().GetField(Name, BindingFlags.NonPublic | BindingFlags.Public |
                    BindingFlags.Instance | BindingFlags.Static);
                if (field != null)
                {
                    return field.GetValue(target);
                }
                var property = target.GetType().GetProperty(Name, BindingFlags.NonPublic | BindingFlags.Public |
                    BindingFlags.Instance | BindingFlags.Static);
                if (property != null)
                {
                    return property.GetValue(target);
                }
                return null;
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
            public override bool Check(object target) => this.Value.Equals(this.InjectGetValue(target));
            public IsAttribute(string Name, object value) : base(Name, value) { }
        }
        [System.AttributeUsage(AttributeTargets.Field | AttributeTargets.Property, Inherited = true, AllowMultiple = true)]
        public class NotAttribute : WhenMemberValueAttribute
        {
            public override bool Check(object target) => !this.Value.Equals(this.InjectGetValue(target));
            public NotAttribute(string Name, object value) : base(Name, value) { }
        }
    }
    [System.AttributeUsage(AttributeTargets.All, Inherited = false, AllowMultiple = false)]
    public class IgnoreAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.All, Inherited = false, AllowMultiple = false)]
    public class SettingAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.All, Inherited = false, AllowMultiple = false)]
    public class ResourcesAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.All, Inherited = false, AllowMultiple = false)]
    public class ContentAttribute : Attribute { }
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
            if(property != null)
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
        public HopeNotNullAttribute() {}
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


    public static partial class ConventionUtility
    {
        public static void InitExtensionEnv()
        {
            RegisterBaseWrapperExtension.InitExtensionEnv();

            UnityExtension.InitExtensionEnv();

            ES3Plugin.InitExtensionEnv();
        }

        public static bool IsNumber(object data)
        {
            if (data == null) return false;
            var type = data.GetType();
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
        public static bool IsString(object data)
        {
            if (data == null) return false;
            var type = data.GetType();
            return type == typeof(string) || type == typeof(char[]);
        }
        public static bool IsBinary(object data)
        {
            if (data == null) return false;
            var type = data.GetType();
            return
                type == typeof(byte) ||
                type == typeof(sbyte) ||
                type == typeof(byte[]) ||
                type == typeof(sbyte[]);
        }
        public static bool IsArray(object data)
        {
            if (data == null) return false;
            var type = data.GetType();
            return type.IsArray;
        }
        public static bool IsBool(object data)
        {
            if (data == null) return false;
            return data.GetType() == typeof(bool);
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
}
