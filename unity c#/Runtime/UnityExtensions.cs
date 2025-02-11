using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Convention
{
    public static class UnityExtension
    {
        public static void InitExtensionEnv()
        {
            AsyncOperationExtension.InitExtensionEnv();
        }
    }

    public static class AsyncOperationExtension
    {
        public static void InitExtensionEnv()
        {
            CompletedHelper.InitExtensionEnv();
        }
        public static void MarkCompleted(this AsyncOperation operation, [In] Action action)
        {
            operation.completed += new CompletedHelper(action).InternalCompleted;
        }

        private class CompletedHelper
        {
            static CompletedHelper() => helpers = new();
            public static void InitExtensionEnv() => helpers.Clear();
            private static readonly List<CompletedHelper> helpers = new();

            readonly Action action;

            public CompletedHelper([In]Action action)
            {
                helpers.Add(this);
                this.action = action;
            }
            ~CompletedHelper()
            {
                helpers.Remove(this);
            }

            public void InternalCompleted(AsyncOperation obj)
            {
                if (obj.progress < 0.99f) return;
                action.Invoke();
                helpers.Remove(this);
            }

        }
    }
}