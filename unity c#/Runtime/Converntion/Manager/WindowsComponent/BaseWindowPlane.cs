using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

namespace Convention
{
    public class BaseWindowPlane : MonoAnyBehaviour
    {
        private void Reset()
        {
            string str = "";
            str = Plane?.ToString();
        }

        [Resources, SerializeField, HopeNotNull] private Image m_Plane;
        [Setting]
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
    }
}
