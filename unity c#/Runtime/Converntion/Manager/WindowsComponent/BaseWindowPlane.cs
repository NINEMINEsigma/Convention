using System;
using UnityEngine;
using UnityEngine.UI;

namespace Convention.WindowsUI
{
    public class BaseWindowPlane : WindowsComponent
    {
        [Resources, SerializeField, OnlyNotNullMode] private Image m_Plane;
        [Resources, SerializeField, HopeNotNull, Tooltip("This animational plane should has the same parent transform")]
        private Image m_AnimationPlane;
        [Setting, OnlyNotNullMode(nameof(m_AnimationPlane)), SerializeField, Header("Animation Setting")]
        private bool IsEnableAnimation = true;
        [Setting, OnlyNotNullMode(nameof(m_AnimationPlane)), Percentage(0, 1), Range(0, 1), WhenAttribute.Is(nameof(IsEnableAnimation), true)]
        public float AnimationSpeed = 0.5f;

        [Content, OnlyPlayMode, Ignore] public RectTransformInfo BeforeMaximizeWindow = null;
        private bool IsMaximizeWindowMode = false;
        public void MaximizeWindow()
        {
            if (IsMaximizeWindowMode)
                return;
            BeforeMaximizeWindow = new(m_Plane.rectTransform);
            var prect = m_Plane.transform.parent.GetComponent<RectTransform>();
            m_Plane.rectTransform.SetPositionAndRotation(prect.position, prect.rotation);
            m_Plane.rectTransform.anchoredPosition = Vector3.zero;
            m_Plane.rectTransform.anchorMax = Vector2.one;
            m_Plane.rectTransform.anchorMin = Vector2.zero;
            m_Plane.rectTransform.sizeDelta = Vector2.zero;
            IsMaximizeWindowMode = true;
        }
        public void ExitMaximizeWindowMode()
        {
            if (!IsMaximizeWindowMode)
                return;
            BeforeMaximizeWindow.Setup(m_Plane.rectTransform);
            IsMaximizeWindowMode = false;
        }

        private void OnEnable()
        {
            if (m_AnimationPlane != null)
            {
                new RectTransformInfo(m_Plane.rectTransform).Setup(m_AnimationPlane.rectTransform);
            }
        }
        private void FixedUpdate()
        {
            if (IsEnableAnimation && m_AnimationPlane != null)
            {
                RectTransformInfo.UpdateAnimationPlane(m_Plane.rectTransform, m_AnimationPlane.GetComponent<RectTransform>(),
                    AnimationSpeed, IsMaximizeWindowMode ? 1 : -1);
            }
        }
    }
}
