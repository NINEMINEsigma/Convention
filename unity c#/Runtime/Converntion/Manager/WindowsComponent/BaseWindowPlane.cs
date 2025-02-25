using System;
using UnityEngine;
using UnityEngine.UI;

namespace Convention.WindowsUI
{
    public class BaseWindowPlane : WindowsComponent
    {
        [Resources, SerializeField, OnlyNotNullMode] private RectTransform m_Plane;
        [Resources, SerializeField, HopeNotNull, Tooltip("This animational plane should has the same parent transform")]
        private RectTransform m_AnimationPlane;
        [Setting, OnlyNotNullMode(nameof(m_AnimationPlane)), SerializeField, Header("Animation Setting")]
        private bool IsEnableAnimation = true;
        [Setting, OnlyNotNullMode(nameof(m_AnimationPlane)), Percentage(0, 1), Range(0, 1), WhenAttribute.Is(nameof(IsEnableAnimation), true)]
        public float AnimationSpeed = 0.5f;

        [Content, OnlyPlayMode, Ignore] public RectTransformInfo BeforeMaximizeWindow = null;
        private bool IsMaximizeWindowMode = false;
        [Content, OnlyPlayMode]
        public void MaximizeWindow()
        {
            if (IsMaximizeWindowMode)
                return;
            BeforeMaximizeWindow = new(m_Plane);
            var prect = m_Plane.transform.parent.GetComponent<RectTransform>();
            m_Plane.SetPositionAndRotation(prect.position, prect.rotation);
            m_Plane.anchoredPosition = Vector3.zero;
            m_Plane.anchorMax = Vector2.one;
            m_Plane.anchorMin = Vector2.zero;
            m_Plane.sizeDelta = Vector2.zero;
            IsMaximizeWindowMode = true;
        }
        [Content, OnlyPlayMode]
        public void ExitMaximizeWindowMode()
        {
            if (!IsMaximizeWindowMode)
                return;
            BeforeMaximizeWindow.Setup(m_Plane);
            IsMaximizeWindowMode = false;
        }

        private void OnEnable()
        {
            if (m_AnimationPlane != null)
            {
                new RectTransformInfo(m_Plane).Setup(m_AnimationPlane);
            }
        }
        [Content]
        public void SynchronizedAnimationPlane()
        {
            new RectTransformInfo(m_Plane).Setup(m_AnimationPlane);
        }

        private void FixedUpdate()
        {
            if (IsEnableAnimation && m_Plane && m_AnimationPlane)
            {
                RectTransformInfo.UpdateAnimationPlane(m_Plane, m_AnimationPlane, AnimationSpeed, IsMaximizeWindowMode ? 1 : -1);
            }
        }

        public void AddChild(RectTransform target, Rect rect, bool isAdjustSizeToContainsChilds = false)
        {
            RectTransformInfo.SetParentAndResize(rectTransform, target, rect, isAdjustSizeToContainsChilds);
        }
        public void AddChild(RectTransform target, bool isAdjustSizeToContainsChilds = false)
        {
            RectTransformInfo.SetParentAndResize(rectTransform, target, isAdjustSizeToContainsChilds);
        }
    }
}
