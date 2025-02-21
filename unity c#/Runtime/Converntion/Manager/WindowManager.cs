using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Convention.WindowsUI;

namespace Convention
{
    namespace WindowsUI
    {
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

        [Resources,SerializeField, HopeNotNull] private BaseWindowPlane m_WindowPlane;
        public BaseWindowPlane WindowPlane
        {
            get
            {
                if (m_WindowPlane == null && GetComponents<BaseWindowPlane>().Length != 0)
                {
                    m_WindowPlane = GetComponents<BaseWindowPlane>()[0];
                }
                return m_WindowPlane;
            }
        }
    }
}
