using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

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
        }
        public class BaseWindowPlane : WindowsComponent
        {
            private void Reset()
            {
                string str = "";
                str = Plane?.ToString();
            }

            [Resources, SerializeField, HopeNotNull] private Image m_Plane;
            [Resources, SerializeField, HopeNotNull] private Image m_AnimationPlane;
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
            public Image AnimationPlane => m_AnimationPlane;

            [Content, OnlyPlayMode, Ignore] public RectTransformInfo BeforeMaximizeWindow = null;
            [Content, OnlyPlayMode, Ignore] public RectTransformInfo AfterMaximizeWindow = null;
            public void MaximizeWindow(bool isMax = true)
            {
                if (isMax == false)
                {
                    ExitMaximizeWindowMode();
                    return;
                }
                BeforeMaximizeWindow = new(this.rectTransform);
                if (AfterMaximizeWindow == null)
                {
                    //this.rectTransform.SetLocalPositionAndRotation(Vector3.zero, Quaternion.identity);
                    //this.rectTransform.anchoredPosition = Vector3.zero;
                    //this.rectTransform.anchorMax = new(0.5f, 0.5f);
                    //this.rectTransform.anchorMin = new(0.5f, 0.5f);
                    //this.rectTransform.sizeDelta = this.transform.parent.GetComponent<RectTransform>().sizeDelta;
                    var prect = this.transform.parent.GetComponent<RectTransform>();
                    this.rectTransform.SetPositionAndRotation(prect.position, prect.rotation);
                    this.rectTransform.anchoredPosition = Vector3.zero;
                    this.rectTransform.anchorMax = Vector2.one;
                    this.rectTransform.anchorMin = Vector2.zero;
                    this.rectTransform.sizeDelta = Vector2.zero;
                }
                AfterMaximizeWindow.Setup(this.rectTransform);
            }
            public void ExitMaximizeWindowMode()
            {
                BeforeMaximizeWindow.Setup(this.rectTransform);
                BeforeMaximizeWindow = null;
            }
        }
    }
}
