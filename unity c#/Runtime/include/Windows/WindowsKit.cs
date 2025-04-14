using System;
using System.IO;
using System.Runtime.InteropServices;

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

        [DllImport("Comdlg32.dll", SetLastError = true, ThrowOnUnmappableChar = true, CharSet = CharSet.Auto)]
        public static extern bool GetOpenFileName([In, Out] OpenFileName ofn);

        [DllImport("Comdlg32.dll", SetLastError = true, ThrowOnUnmappableChar = true, CharSet = CharSet.Auto)]
        public static extern bool GetSaveFileName([In, Out] OpenFileName ofn);

        [DllImport("shell32.dll", CharSet = CharSet.Auto)]
        public static extern IntPtr SHBrowseForFolder(ref BROWSEINFO lpbi);

        [DllImport("shell32.dll", CharSet = CharSet.Auto)]
        public static extern bool SHGetPathFromIDList(IntPtr pidl, IntPtr pszPath);

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
        public struct BROWSEINFO
        {
            public IntPtr hwndOwner;
            public IntPtr pidlRoot;
            public string pszDisplayName;
            public string lpszTitle;
            public uint ulFlags;
            public IntPtr lpfn;
            public IntPtr lParam;
            public int iImage;
        }

        public static string SelectFolder(string description = "请选择文件夹")
        {
            BROWSEINFO bi = new BROWSEINFO();
            bi.lpszTitle = description;
            bi.ulFlags = 0x00000040; // BIF_NEWDIALOGSTYLE
            bi.hwndOwner = IntPtr.Zero;

            IntPtr pidl = SHBrowseForFolder(ref bi);
            if (pidl != IntPtr.Zero)
            {
                IntPtr pathPtr = Marshal.AllocHGlobal(260);
                if (SHGetPathFromIDList(pidl, pathPtr))
                {
                    string path = Marshal.PtrToStringAuto(pathPtr);
                    Marshal.FreeHGlobal(pathPtr);
                    current_initialDir = path;
                    return path;
                }
                Marshal.FreeHGlobal(pathPtr);
            }
            return null;
        }

        public static string[] SelectMultipleFiles(string filter = "所有文件|*.*", string title = "选择文件")
        {
            OpenFileName ofn = new OpenFileName();
            ofn.structSize = Marshal.SizeOf(ofn);
            ofn.filter = filter.Replace("|", "\0") + "\0";
            ofn.file = new string(new char[256]);
            ofn.maxFile = ofn.file.Length;
            ofn.fileTitle = new string(new char[64]);
            ofn.maxFileTitle = ofn.fileTitle.Length;
            ofn.initialDir = current_initialDir;
            ofn.title = title;
            ofn.flags = 0x00080000 | 0x00001000 | 0x00000800 | 0x00000008 | 0x00000200; // OFN_ALLOWMULTISELECT

            if (GetOpenFileName(ofn))
            {
                current_initialDir = Path.GetDirectoryName(ofn.file);
                return ofn.file.Split('\0');
            }
            return null;
        }

        public static string SaveFile(string filter = "所有文件|*.*", string title = "保存文件")
        {
            OpenFileName ofn = new OpenFileName();
            ofn.structSize = Marshal.SizeOf(ofn);
            ofn.filter = filter.Replace("|", "\0") + "\0";
            ofn.file = new string(new char[256]);
            ofn.maxFile = ofn.file.Length;
            ofn.fileTitle = new string(new char[64]);
            ofn.maxFileTitle = ofn.fileTitle.Length;
            ofn.initialDir = current_initialDir;
            ofn.title = title;
            ofn.flags = 0x00080000 | 0x00001000 | 0x00000800 | 0x00000008 | 0x00000002; // OFN_OVERWRITEPROMPT

            if (GetSaveFileName(ofn))
            {
                current_initialDir = Path.GetDirectoryName(ofn.file);
                return ofn.file;
            }
            return null;
        }
    }
}
