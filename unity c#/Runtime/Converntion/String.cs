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

    }
}
