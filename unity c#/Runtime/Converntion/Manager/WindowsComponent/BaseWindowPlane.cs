using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

namespace Convention
{
    namespace WindowsUI
    {
        public class BaseWindowPlane : WindowsComponent
        {
            private void Reset()
            {
                string str = "";
                str = Plane?.ToString();
            }

            [Resources, SerializeField, HopeNotNull] private Image m_Plane;
            [Resources, SerializeField, HopeNotNull, Tooltip("This animational plane should has the same parent transform")]
            private Image m_AnimationPlane;
            [Setting, OnlyNotNullMode(nameof(m_AnimationPlane)), SerializeField, Header("Animation Setting")]
            private bool IsEnableAnimation = true;
            [Setting, OnlyNotNullMode(nameof(m_AnimationPlane)), Percentage(0, 1), Range(0, 1), WhenAttribute.Is(nameof(IsEnableAnimation), true)]
            public float AnimationSpeed = 0.5f;

            public Image Plane
            {
                get
                {
                    if (m_Plane == null)
                    {
                        m_Plane = GetComponent<Image>();
                    }
                    return m_Plane;
                }
            }

            [Content, OnlyPlayMode, Ignore] public RectTransformInfo BeforeMaximizeWindow = null;
            private bool IsMaximizeWindowMode = false;
            public void MaximizeWindow()
            {
                if (IsMaximizeWindowMode)
                    return;
                BeforeMaximizeWindow = new(this.rectTransform);
                var prect = this.transform.parent.GetComponent<RectTransform>();
                this.rectTransform.SetPositionAndRotation(prect.position, prect.rotation);
                this.rectTransform.anchoredPosition = Vector3.zero;
                this.rectTransform.anchorMax = Vector2.one;
                this.rectTransform.anchorMin = Vector2.zero;
                this.rectTransform.sizeDelta = Vector2.zero;
                IsMaximizeWindowMode = true;
            }
            public void ExitMaximizeWindowMode()
            {
                if (!IsMaximizeWindowMode)
                    return;
                BeforeMaximizeWindow.Setup(this.rectTransform);
                IsMaximizeWindowMode = false;
            }

            private void OnEnable()
            {
                if (m_AnimationPlane != null)
                {
                    new RectTransformInfo(this.rectTransform).Setup(m_AnimationPlane.rectTransform);
                }
            }
            private void FixedUpdate()
            {
                if (IsEnableAnimation && m_AnimationPlane != null)
                {
                    RectTransformInfo.UpdateAnimationPlane(rectTransform, m_AnimationPlane.GetComponent<RectTransform>(),
                        AnimationSpeed, IsMaximizeWindowMode ? 1 : -1);
                }
            }
        }
    }
}
