using System;
using System.Runtime.InteropServices;
using Convention.Internal;

namespace Convention
{
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

    namespace Internal
    {
        public class Indicator
        {
            public static Type tag { get; protected set; }
            public static bool value { get; protected set; }
        }
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
        public WhenAttribute([In] object return_value_is_equal_this_value_or_pr_is_return_true, [In] Type typenAttribute)
        {
            this.TypenAttribute = typenAttribute;
            var prm = return_value_is_equal_this_value_or_pr_is_return_true.GetType().GetMethod("Invoke");
            if (prm != null &&
                prm.GetParameters().Length == 1 &&
                prm.ReturnType == typeof(bool))
                this.pr = (SALCheckException.Predicate)return_value_is_equal_this_value_or_pr_is_return_true;
            else
                this.pr = (object obj) => obj == return_value_is_equal_this_value_or_pr_is_return_true;
        }
        public WhenAttribute([In] string description) { }

        public bool CheckAnyValue([In][Opt] object value)
        {
            if (pr == null)
                return true;
            if (this.pr(value))
                return true;
            return false;
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
            UnityExtension.InitExtensionEnv();
        }

        public static bool IsNumber<T>(T data)
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
        public static bool IsString<T>(T data)
        {
            if (data == null) return false;
            var type = data.GetType();
            return type == typeof(string) || type == typeof(char[]);
        }
        public static bool IsBinary<T>(T data)
        {
            if (data == null) return false;
            var type = data.GetType();
            return
                type == typeof(byte) ||
                type == typeof(sbyte) ||
                type == typeof(byte[]) ||
                type == typeof(sbyte[]);
        }
        public static bool IsArray<T>(T data)
        {
            if (data == null) return false;
            var type = data.GetType();
            return type.IsArray;
        }
        public static bool IsBool<T>(T data)
        {
            if (data == null) return false;
            return data.GetType() == typeof(bool);
        }
    }
}
