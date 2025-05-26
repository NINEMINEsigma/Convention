using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace Convention
{
    public static class StringExtension
    {
        public static void InitExtensionEnv()
        {
            CurrentStringTransformer = null;
            MyLazyTransformer.Clear();
        }

        public static string LimitString([In] object data, int maxLength = 50)
        {
            return LimitString(data.ToString(), maxLength);
        }
        public static string LimitString([In] in string data, int maxLength = 50)
        {
            if (data.Length <= maxLength)
                return data;
            var insideStr = "\n...\n...\n";
            int headLength = maxLength / 2;
            int tailLength = maxLength - headLength - insideStr.Length;
            return data[..headLength] + insideStr + data[^tailLength..];
        }

        public enum Side
        {
            Left,
            Right,
            Center
        }
        public static string FillString([In] object data, int maxLength = 50, char fillChar = ' ', Side side = Side.Right)
        {
            return FillString(data.ToString(), maxLength, fillChar, side);
        }
        public static string FillString([In] in string data, int maxLength = 50, char fillChar = ' ', Side side = Side.Right)
        {
            if (data.Length >= maxLength)
                return data;
            var fillStr = new string(fillChar, maxLength - data.Length);
            switch (side)
            {
                case Side.Left:
                    return fillStr + data;
                case Side.Right:
                    return data + fillStr;
                case Side.Center:
                    int leftLength = (maxLength - data.Length) / 2;
                    int rightLength = maxLength - leftLength - data.Length;
                    return new string(fillChar, leftLength) + data + new string(fillChar, rightLength);
                default:
                    return data;
            }
        }

        public static List<string> BytesToStrings([In] IEnumerable<byte[]> bytes)
        {
            return BytesToStrings(bytes, Encoding.UTF8);
        }
        public static List<string> BytesToStrings([In] IEnumerable<byte[]> bytes, Encoding encoding)
        {
            return bytes.ToList().ConvertAll(x => encoding.GetString(x));
        }

        private static Dictionary<string, string> MyLazyTransformer = new();

        public class StringTransformer : AnyClass
        {
            [Serializable,ArgPackage]
            public class StringContentTree
            {
                public string leaf = null;
                public Dictionary<string, StringContentTree> branch = null;
            }

            private StringContentTree contents;

            public StringTransformer([In] string transformerFile)
            {
                var file = new ToolFile(transformerFile);
                contents = file.LoadAsRawJson<StringContentTree>();
            }

            public string Transform([In] string stringName)
            {
                if (contents == null || contents.branch == null)
                    return stringName;
                var keys = stringName.Split('.');
                StringContentTree current = contents;
                foreach (var k in keys)
                {
                    if (current.branch != null && current.branch.TryGetValue(k, out var next))
                    {
                        current = next;
                    }
                    else
                    {
                        return stringName; // If any key is not found, return the original key
                    }
                }
                return current.leaf ?? stringName; // Return leaf or original key if leaf is null
            }
        }

        private static StringTransformer MyCurrentStringTransformer = null;
        public static StringTransformer CurrentStringTransformer
        {
            get => MyCurrentStringTransformer;
            set
            {
                if (MyCurrentStringTransformer != value)
                {
                    MyLazyTransformer.Clear();
                    MyCurrentStringTransformer = value;
                }
            }
        }

        public static string Transform([In] string stringName)
        {
            if (MyLazyTransformer.TryGetValue(stringName, out var result))
                return result;
            return MyLazyTransformer[stringName] = CurrentStringTransformer != null
                ? CurrentStringTransformer.Transform(stringName)
                : stringName;
        }
    }
}
