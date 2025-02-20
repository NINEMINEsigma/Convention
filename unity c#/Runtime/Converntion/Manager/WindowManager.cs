using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Convention
{
    namespace WindowsUI
    {
        public class WindowsComponent : MonoAnyBehaviour
        {

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
