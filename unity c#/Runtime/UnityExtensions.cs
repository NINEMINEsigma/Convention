using System;
using System.Collections;
using System.Collections.Generic;
using Convention.WindowsUI;
using UnityEngine;
using UnityEngine.UI;

namespace Convention
{
    public static class UnityExtension
    {
        public static void InitExtensionEnv()
        {
            AsyncOperationExtension.InitExtensionEnv();
            RectTransformExtension.InitExtensionEnv();
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

            public CompletedHelper([In] Action action)
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

    public static partial class RectTransformExtension
    {
        private static bool IsDisableAdjustSizeToContainsChilds2DeferUpdates = false;

        public static void InitExtensionEnv()
        {
            IsDisableAdjustSizeToContainsChilds2DeferUpdates = false;
        }

        public class AdjustSizeIgnore : MonoAnyBehaviour { }
        public static void DisableAdjustSizeToContainsChilds2DeferUpdates()
        {
            IsDisableAdjustSizeToContainsChilds2DeferUpdates = true;
        }
        public static void AppleAndEnableAdjustSizeToContainsChilds()
        {
            IsDisableAdjustSizeToContainsChilds2DeferUpdates = false;
        }
        public static void AdjustSizeToContainsChilds([In] RectTransform rectTransform, Vector2 min, Vector2 max, RectTransform.Axis? axis)
        {
            if (IsDisableAdjustSizeToContainsChilds2DeferUpdates)
                return;
            LayoutRebuilder.ForceRebuildLayoutImmediate(rectTransform);

            bool stats = false;

            List<RectTransform> currentList = new(), nextList = new();
            foreach (RectTransform item in rectTransform)
            {
                currentList.Add(item);
            }
            do
            {
                currentList.AddRange(nextList);
                nextList.Clear();
                foreach (RectTransform childTransform in currentList)
                {
                    if (childTransform.gameObject.activeInHierarchy == false)
                        continue;
                    if (childTransform.name.ToLower().Contains("<ignore rect>"))
                        continue;
                    if (childTransform.GetComponents<AdjustSizeIgnore>().Length != 0)
                        continue;
                    stats = true;
                    foreach (RectTransform item in childTransform)
                    {
                        nextList.Add(item);
                    }
                    var corners = new Vector3[4];
                    childTransform.GetWorldCorners(corners);

                    foreach (var corner in corners)
                    {
                        Vector2 localCorner = rectTransform.InverseTransformPoint(corner);
                        if (float.IsNaN(localCorner.x) || float.IsNaN(localCorner.y))
                            break;
                        min.x = Mathf.Min(min.x, localCorner.x);
                        min.y = Mathf.Min(min.y, localCorner.y);
                        max.x = Mathf.Max(max.x, localCorner.x);
                        max.y = Mathf.Max(max.y, localCorner.y);
                    }
                }
                currentList.Clear();
            } while (nextList.Count > 0);

            if (stats)
            {
                if ((axis.HasValue && axis.Value == RectTransform.Axis.Vertical) ||
                    (rectTransform.anchorMin.x == 0 && rectTransform.anchorMax.x == 1 && rectTransform.anchorMin.y == rectTransform.anchorMax.y))
                {
                    rectTransform.SetSizeWithCurrentAnchors(RectTransform.Axis.Vertical, max.y - min.y);
                }
                else if ((axis.HasValue && axis.Value == RectTransform.Axis.Horizontal) ||
                    (rectTransform.anchorMin.y == 0 && rectTransform.anchorMax.y == 1 && rectTransform.anchorMin.x == rectTransform.anchorMax.x))
                {
                    rectTransform.SetSizeWithCurrentAnchors(RectTransform.Axis.Horizontal, max.x - min.x);
                }
                else
                {
                    rectTransform.SetSizeWithCurrentAnchors(RectTransform.Axis.Horizontal, max.x - min.x);
                    rectTransform.SetSizeWithCurrentAnchors(RectTransform.Axis.Vertical, max.y - min.y);
                }
            }
        }
        public static void AdjustSizeToContainsChilds([In] RectTransform rectTransform, RectTransform.Axis axis)
        {
            if (IsDisableAdjustSizeToContainsChilds2DeferUpdates)
                return;
            Vector2 min = new Vector2(float.MaxValue, float.MaxValue);
            Vector2 max = new Vector2(float.MinValue, float.MinValue);

            AdjustSizeToContainsChilds(rectTransform, min, max, axis);
        }
        public static void AdjustSizeToContainsChilds([In] RectTransform rectTransform)
        {
            if (IsDisableAdjustSizeToContainsChilds2DeferUpdates)
                return;
            Vector2 min = new Vector2(float.MaxValue, float.MaxValue);
            Vector2 max = new Vector2(float.MinValue, float.MinValue);

            AdjustSizeToContainsChilds(rectTransform, min, max, null);
        }

        internal static void SetParentAndResizeWithoutNotifyBaseWindowPlane([In] RectTransform parent, [In] RectTransform child, Rect rect, bool isAdjustSizeToContainsChilds)
        {
            child.SetParent(parent);
            child.SetInsetAndSizeFromParentEdge(RectTransform.Edge.Left, rect.x, rect.width);
            child.SetInsetAndSizeFromParentEdge(RectTransform.Edge.Top, rect.y, rect.height);
            if (isAdjustSizeToContainsChilds)
                AdjustSizeToContainsChilds(parent);
        }
        internal static void SetParentAndResizeWithoutNotifyBaseWindowPlane(RectTransform parent, RectTransform child, bool isAdjustSizeToContainsChilds)
        {
            child.SetParent(parent);
            if (isAdjustSizeToContainsChilds)
                AdjustSizeToContainsChilds(parent);
        }
        public static void SetParentAndResize(RectTransform parent, RectTransform child, Rect rect, bool isAdjustSizeToContainsChilds)
        {
            if (parent.GetComponents<BaseWindowPlane>().Length != 0)
            {
                parent.GetComponents<BaseWindowPlane>()[0].AddChild(child, rect, isAdjustSizeToContainsChilds);
            }
            else
            {
                SetParentAndResizeWithoutNotifyBaseWindowPlane(parent, child, rect, isAdjustSizeToContainsChilds);
            }
        }
        public static void SetParentAndResize(RectTransform parent, RectTransform child, bool isAdjustSizeToContainsChilds)
        {
            if (parent.GetComponents<BaseWindowPlane>().Length != 0)
            {
                parent.GetComponents<BaseWindowPlane>()[0].AddChild(child, isAdjustSizeToContainsChilds);
            }
            else
            {
                SetParentAndResizeWithoutNotifyBaseWindowPlane(parent, child, isAdjustSizeToContainsChilds);
            }
        }
    }
}
