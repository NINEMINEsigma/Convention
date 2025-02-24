using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Convention.WindowsUI;
using System;
using UnityEngine.Rendering;
using System.Linq;
using UnityEngine.UI;
using UnityEngine.Events;

namespace Convention
{
    namespace WindowsUI
    {
        [ArgPackage, Serializable]
        public class RectTransformInfo : AnyClass
        {
            [Setting] public Vector2 position;
            [Setting] public Vector2 anchoredPosition;
            [Setting] public Quaternion rotation;
            [Setting] public Vector2 sizeDelta;
            [Setting] public Vector2 anchorMax;
            [Setting] public Vector2 anchorMin;
            [Setting] public Vector2 pivot;

            public RectTransformInfo(RectTransform rect)
            {
                position = rect.position;
                rotation = rect.rotation;
                anchoredPosition = rect.anchoredPosition;
                sizeDelta = rect.sizeDelta;
                anchorMax = rect.anchorMax;
                anchorMin = rect.anchorMin;
                pivot = rect.pivot;
            }
            public void Setup(RectTransform rect)
            {
                rect.position = position;
                rect.rotation = rotation;
                rect.anchoredPosition = anchoredPosition;
                rect.sizeDelta = sizeDelta;
                rect.anchorMax = anchorMax;
                rect.anchorMin = anchorMin;
                rect.pivot = pivot;
            }
            protected static float AnimationalAdd(float value, float additional, float from, float to)
            {
                float min = Mathf.Min(from, to);
                float max = Mathf.Max(from, to);
                return Mathf.Clamp(value + additional, min, max);
            }
            protected static float AnimationalPercentage([Percentage(0, 1)] float percentage, float from, float to, float minimumThresholdValue)
            {
                float min = Mathf.Min(from, to);
                float max = Mathf.Max(from, to);
                return Mathf.Clamp(Mathf.Lerp(from, to, percentage) + minimumThresholdValue, min, max);
            }
            protected static float AnimationalPercentageAngle([Percentage(0, 1)] float percentage, float from, float to, float minimumThresholdValue)
            {
                float min = Mathf.Min(from, to);
                float max = Mathf.Max(from, to);
                return Mathf.Clamp(Mathf.LerpAngle(from, to, percentage) + minimumThresholdValue, min, max);
            }
            /// <summary>
            /// update until they as the same
            /// </summary>
            /// <param name="rectTransform"></param>
            /// <param name="animationTran"></param>
            /// <param name="speed"></param>
            /// <returns>is same</returns>
            public static void UpdateAnimationPlane(RectTransform rectTransform, RectTransform animationTran, float speed, float minimumThresholdValue)
            {
                animationTran.position = new(
                    Lerp(rectTransform.position.x, animationTran.position.x),
                    Lerp(rectTransform.position.y, animationTran.position.y),
                    Lerp(rectTransform.position.z, animationTran.position.z));
                animationTran.eulerAngles = (new(
                    LerpAngle(rectTransform.rotation.eulerAngles.x, animationTran.rotation.eulerAngles.x),
                    LerpAngle(rectTransform.rotation.eulerAngles.y, animationTran.rotation.eulerAngles.y),
                    LerpAngle(rectTransform.rotation.eulerAngles.z, animationTran.rotation.eulerAngles.z)));

                minimumThresholdValue = 0;
                animationTran.pivot = new(
                    Lerp(rectTransform.pivot.x, animationTran.pivot.x),
                    Lerp(rectTransform.pivot.y, animationTran.pivot.y));
                animationTran.anchoredPosition = new(
                    Lerp(rectTransform.anchoredPosition.x, animationTran.anchoredPosition.x),
                    Lerp(rectTransform.anchoredPosition.y, animationTran.anchoredPosition.y));
                animationTran.sizeDelta = new(
                    Lerp(rectTransform.sizeDelta.x, animationTran.sizeDelta.x),
                    Lerp(rectTransform.sizeDelta.y, animationTran.sizeDelta.y));
                animationTran.anchorMax = new(
                    Lerp(rectTransform.anchorMax.x, animationTran.anchorMax.x),
                    Lerp(rectTransform.anchorMax.y, animationTran.anchorMax.y));
                animationTran.anchorMin = new(
                    Lerp(rectTransform.anchorMin.x, animationTran.anchorMin.x),
                    Lerp(rectTransform.anchorMin.y, animationTran.anchorMin.y));

                float Lerp(float to, float from)
                {
                    return AnimationalPercentage(speed, from, to, minimumThresholdValue);
                }
                float LerpAngle(float to, float from)
                {
                    return AnimationalPercentageAngle(speed, from, to, minimumThresholdValue);
                }
            }

            public static void AdjustSizeToContainsChilds(RectTransform rectTransform, Vector2 min, Vector2 max)
            {
                LayoutRebuilder.ForceRebuildLayoutImmediate(rectTransform);

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
                        foreach (RectTransform item in childTransform)
                        {
                            nextList.Add(item);
                        }
                        var corners = new Vector3[4];
                        childTransform.GetWorldCorners(corners);

                        foreach (var corner in corners)
                        {
                            Vector2 localCorner = rectTransform.InverseTransformPoint(corner);
                            min.x = Mathf.Min(min.x, localCorner.x);
                            min.y = Mathf.Min(min.y, localCorner.y);
                            max.x = Mathf.Max(max.x, localCorner.x);
                            max.y = Mathf.Max(max.y, localCorner.y);
                        }
                    }
                    currentList.Clear();
                } while (currentList.Count > 0);

                //if (rectTransform.anchorMin == rectTransform.anchorMax)
                //{
                //    rectTransform.sizeDelta = new(max.x - min.x, max.y - min.y);
                //}
                //else
                //{
                rectTransform.SetSizeWithCurrentAnchors(RectTransform.Axis.Horizontal, max.x - min.x);
                rectTransform.SetSizeWithCurrentAnchors(RectTransform.Axis.Vertical, min.y - min.y);
                //}
            }
            public static void AdjustSizeToContainsChilds(RectTransform rectTransform)
            {
                Vector2 min = new Vector2(float.MaxValue, float.MaxValue);
                Vector2 max = new Vector2(float.MinValue, float.MinValue);
                AdjustSizeToContainsChilds(rectTransform, min, max);
            }

            public static void SetParentAndResize(RectTransform parent, RectTransform child, Rect rect, bool isAdjustSizeToContainsChilds)
            {
                child.SetParent(parent);
                child.SetInsetAndSizeFromParentEdge(RectTransform.Edge.Left, rect.x, rect.width);
                child.SetInsetAndSizeFromParentEdge(RectTransform.Edge.Top, rect.y, rect.height);
                if (isAdjustSizeToContainsChilds)
                    RectTransformInfo.AdjustSizeToContainsChilds(parent);
            }
            public static void SetParentAndResize(RectTransform parent, RectTransform child, bool isAdjustSizeToContainsChilds)
            {
                if (parent.GetComponents<BaseWindowPlane>().Length != 0)
                {
                    parent.GetComponents<BaseWindowPlane>()[0].AddChild(child, isAdjustSizeToContainsChilds);
                }
                else
                {
                    child.SetParent(parent);
                    if (isAdjustSizeToContainsChilds)
                        RectTransformInfo.AdjustSizeToContainsChilds(parent);
                }
            }
        }
        public class WindowsComponent : MonoAnyBehaviour
        {
            [Resources, Ignore] private RectTransform m_rectTransform;
            public RectTransform rectTransform
            {
                get
                {
                    if (m_rectTransform == null)
                    {
                        m_rectTransform = GetComponent<RectTransform>();
                    }
                    return m_rectTransform;
                }
                protected set
                {
                    m_rectTransform = value;
                }
            }
        }
    }

    public class WindowManager : MonoAnyBehaviour
    {
        private void Reset()
        {
            string str = "";
            str = WindowPlane?.ToString();
        }

        public static WindowManager GenerateWindow()
        {
            return Instantiate(SO.Windows.GlobalInstance.GetWindowsComponent(nameof(WindowManager)).gameObject).GetComponent<WindowManager>();
        }
        [Content, OnlyPlayMode]
        public void CloseWindow()
        {
            throw new NotImplementedException();
        }

        [Content, Ignore, SerializeField] private int m_CurrentContextKey;

        [ArgPackage, Serializable]
        private class ContentEntry : AnyClass
        {
            [OnlyNotNullMode] public RectTransform plane;
            [HopeNotNull] public RectTransform root;
            public GameObject GetRootObject()
            {
                if (root == null)
                    return plane.gameObject;
                return root.gameObject;
            }
        }
        [Resources, SerializeField] private List<ContentEntry> m_AllContextPlane = new();
        [Resources, SerializeField, OnlyNotNullMode] public BaseWindowPlane WindowPlane;
        [Resources, SerializeField] public BaseWindowBar WindowBar;

        public RectTransform CurrentContextRectTransform
        {
            [return: ReturnNotNull]
            get
            {
                if (m_AllContextPlane.Count > 0)
                    return m_AllContextPlane[m_CurrentContextKey].plane;
                if (WindowPlane)
                    return WindowPlane.rectTransform;
                return this.transform as RectTransform;
            }
        }

        public void SelectContextPlane(int key)
        {
            if (CurrentContextRectTransform != this.transform)
            {
                m_AllContextPlane[m_CurrentContextKey].GetRootObject().SetActive(false);
                m_CurrentContextKey = key;
                m_AllContextPlane[m_CurrentContextKey].GetRootObject().SetActive(true);
            }
        }

        [Setting, OnlyPlayMode]
        [return: ReturnNotNull]
        public void SelectNextContextPlane()
        {
            if (m_AllContextPlane.Count > 0)
            {
                m_CurrentContextKey = (m_CurrentContextKey + 1) % m_AllContextPlane.Count;
            }
        }

        public int AddContextPlane([In] RectTransform plane)
        {
            int result = m_AllContextPlane.Count;
            m_AllContextPlane.Add(new() { plane = plane, root = null });
            plane.gameObject.SetActive(false);
            return result;
        }
        public int AddContextPlane([In] RectTransform plane, [In] RectTransform root)
        {
            int result = m_AllContextPlane.Count;
            m_AllContextPlane.Add(new() { plane = plane, root = root });
            root.gameObject.SetActive(false);
            return result;
        }
        [return: ReturnNotNull, When("return current plane's root")]
        public GameObject RemoveContextPlane(int index)
        {
            var result = m_AllContextPlane[index].GetRootObject();
            result.SetActive(false);
            m_AllContextPlane.RemoveAt(index);
            if (m_CurrentContextKey >= index)
                m_CurrentContextKey--;
            m_AllContextPlane[m_CurrentContextKey].GetRootObject().SetActive(true);
            return result;
        }

        public void AddContextChild(RectTransform child, Rect rect, bool isAdjustSizeToContainsChilds)
        {
            RectTransformInfo.SetParentAndResize(CurrentContextRectTransform, child, rect, isAdjustSizeToContainsChilds);
        }
        public void AddContextChild(RectTransform child, bool isAdjustSizeToContainsChilds)
        {
            RectTransformInfo.SetParentAndResize(CurrentContextRectTransform, child, isAdjustSizeToContainsChilds);
        }

        [Content, OnlyPlayMode]
        public BaseWindowBar.RegisteredButtonWrapper CreateWindowBarButton(params UnityAction[] actions)
        {
            var wrapper = WindowBar.RegisterButton();
            wrapper.button.AddListener(actions);
            wrapper.buttonModule.gameObject.SetActive(true);
            return wrapper;
        }
        public BaseWindowBar.RegisteredPageWrapper CreateSubWindowWithBarButton([In][IsInstantiated(true)] RectTransform plane)
        {
            WindowPlane.AddChild(plane, new(0, WindowBar.rectTransform.rect.height, plane.rect.width, plane.rect.height), false);
            var result = WindowBar.RegisterPage(plane);
            result.buttonModule.gameObject.SetActive(true);
            return result;
        }
        public BaseWindowBar.RegisteredPageWrapper CreateSubWindowWithBarButton(
            [In][IsInstantiated(false)] RectTransform plane,
            [In][IsInstantiated(false)] RectTransform root
            )
        {
            WindowPlane.AddChild(plane, new(0, WindowBar.rectTransform.rect.height, plane.rect.width, plane.rect.height), false);
            var result = WindowBar.RegisterPage(plane, root);
            result.buttonModule.gameObject.SetActive(true);
            return result;
        }
    }
}