using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Convention
{
    public static class PluginExtenion
    {
        public static void SelectFileOnSystem(Action<string> action, string labelName, string subLabelName, params string[] fileArgs)
        {
#if PLATFORM_WINDOWS
            WindowsKit.SelectFileOnSystem(action, labelName, subLabelName, fileArgs);
#elif PLATFORM_WEBGL               

#endif
        }
    }
}
