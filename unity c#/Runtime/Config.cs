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
}
