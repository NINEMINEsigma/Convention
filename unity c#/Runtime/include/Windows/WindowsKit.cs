using System;
using System.IO;

namespace Convention
{
    public static class WindowsKit
    {
        public static string current_initialDir = "";

        [System.Runtime.InteropServices.StructLayout(
            System.Runtime.InteropServices.LayoutKind.Sequential, 
            CharSet = System.Runtime.InteropServices.CharSet.Auto)]
        public class OpenFileName
        {
            public int structSize = 0;
            public IntPtr dlgOwner = IntPtr.Zero;
            public IntPtr instance = IntPtr.Zero;
            public string filter = null;
            public string customFilter = null;
            public int maxCustFilter = 0;
            public int filterIndex = 0;
            public string file = null;
            public int maxFile = 0;
            public string fileTitle = null;
            public int maxFileTitle = 0;
            public string initialDir = null;
            public string title = null;
            public int flags = 0;
            public short fileOffset = 0;
            public short fileExtension = 0;
            public string defExt = null;
            public IntPtr custData = IntPtr.Zero;
            public IntPtr hook = IntPtr.Zero;
            public string templateName = null;
            public IntPtr reservedPtr = IntPtr.Zero;
            public int reservedInt = 0;
            public int flagsEx = 0;
        }

        public class LocalDialog
        {
            [System.Runtime.InteropServices.DllImport(
                "Comdlg32.dll", SetLastError = true,
                ThrowOnUnmappableChar = true, 
                CharSet = System.Runtime.InteropServices.CharSet.Auto)]
            public static extern bool GetOpenFileName(
                [System.Runtime.InteropServices.In, System.Runtime.InteropServices.Out] OpenFileName ofn);
            public static bool GetOFN(
                [System.Runtime.InteropServices.In, System.Runtime.InteropServices.Out] OpenFileName ofn)
            {
                return GetOpenFileName(ofn);
            }

            [System.Runtime.InteropServices.DllImport(
                "Comdlg32.dll", SetLastError = true,
                ThrowOnUnmappableChar = true,
                CharSet = System.Runtime.InteropServices.CharSet.Auto)]
            public static extern bool GetSaveFileName(
                [System.Runtime.InteropServices.In, System.Runtime.InteropServices.Out] OpenFileName ofn);
            public static bool GetSFN(
                [System.Runtime.InteropServices.In, System.Runtime.InteropServices.Out] OpenFileName ofn)
            {
                return GetSaveFileName(ofn);
            }
        }

        public static OpenFileName SelectFileOnSystem(string labelName, string subLabelName, params string[] fileArgs)
        {
            OpenFileName targetFile = new OpenFileName();
            targetFile.structSize = System.Runtime.InteropServices.Marshal.SizeOf(targetFile);
            targetFile.filter = labelName + "(*" + subLabelName + ")\0";
            for (int i = 0; i < fileArgs.Length - 1; i++)
            {
                targetFile.filter += "*." + fileArgs[i] + ";";
            }
            if (fileArgs.Length > 0) targetFile.filter += "*." + fileArgs[^1] + ";\0";
            targetFile.file = new string(new char[256]);
            targetFile.maxFile = targetFile.file.Length;
            targetFile.fileTitle = new string(new char[64]);
            targetFile.maxFileTitle = targetFile.fileTitle.Length;
            targetFile.initialDir = current_initialDir;
            targetFile.title = "Select";
            targetFile.flags = 0x00080000 | 0x00001000 | 0x00000800 | 0x00000008;
            return targetFile;
        }

        public static OpenFileName SelectFileOnSystem(Action<string> action, string labelName, string subLabelName, params string[] fileArgs)
        {
            OpenFileName targetFile = SelectFileOnSystem(labelName, subLabelName, fileArgs);
            if (LocalDialog.GetOpenFileName(targetFile) && targetFile.file != "")
            {
                action(targetFile.file);
                current_initialDir = new FileInfo(targetFile.file).Directory.FullName;
            }
            return targetFile;
        }
    }
}
