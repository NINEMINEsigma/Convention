using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Convention.WindowsUI;
using System;

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

            public RectTransformInfo(RectTransform rect)
            {
                position = rect.position;
                rotation = rect.rotation;
                anchoredPosition = rect.anchoredPosition;
                sizeDelta = rect.sizeDelta;
                anchorMax = rect.anchorMax;
                anchorMin = rect.anchorMin;
            }
            public void Setup(RectTransform rect)
            {
                rect.position = position;
                rect.rotation = rotation;
                rect.anchoredPosition = anchoredPosition;
                rect.sizeDelta = sizeDelta;
                rect.anchorMax = anchorMax;
                rect.anchorMin = anchorMin;
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
            public static bool UpdateAnimationPlane(RectTransform rectTransform, RectTransform animationTran, float speed, float minimumThresholdValue)
            {
                bool stats = true;
                if (
                    !Mathf.Approximately(0, (animationTran.position - rectTransform.position).magnitude) ||
                    !Mathf.Approximately(0, (animationTran.eulerAngles - rectTransform.eulerAngles).magnitude)
                    )
                {
                    animationTran.position = new(
                        Lerp(rectTransform.position.x, animationTran.position.x),
                        Lerp(rectTransform.position.y, animationTran.position.y),
                        Lerp(rectTransform.position.z, animationTran.position.z));
                    animationTran.eulerAngles = (new(
                        LerpAngle(rectTransform.rotation.eulerAngles.x, animationTran.rotation.eulerAngles.x),
                        LerpAngle(rectTransform.rotation.eulerAngles.y, animationTran.rotation.eulerAngles.y),
                        LerpAngle(rectTransform.rotation.eulerAngles.z, animationTran.rotation.eulerAngles.z)));
                    stats = false;
                }
                minimumThresholdValue = 0;
                if (
                    !Mathf.Approximately(0, (animationTran.anchoredPosition - rectTransform.anchoredPosition).magnitude) ||
                    !Mathf.Approximately(0, (animationTran.sizeDelta - rectTransform.sizeDelta).magnitude) ||
                    !Mathf.Approximately(0, (animationTran.anchorMax - rectTransform.anchorMax).magnitude) ||
                    !Mathf.Approximately(0, (animationTran.anchorMin - rectTransform.anchorMin).magnitude)
                    )
                {
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
                    stats = false;
                }
                return stats;
                float Lerp(float to, float from)
                {
                    return AnimationalPercentage(speed, from, to, minimumThresholdValue);
                }
                float LerpAngle(float to, float from)
                {
                    return AnimationalPercentageAngle(speed, from, to, minimumThresholdValue);
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
            throw new NotImplementedException();    
        }
        public void CloseWindow()
        {
            throw new NotImplementedException();
        }

        [Resources, SerializeField, OnlyNotNullMode] public BaseWindowPlane WindowPlane;
        [Resources, SerializeField] public BaseWindowBar WindowBar;
    }
}
