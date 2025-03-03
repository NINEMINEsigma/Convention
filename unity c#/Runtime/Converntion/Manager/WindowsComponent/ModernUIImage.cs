using UnityEngine;
using UnityEngine.UI;

namespace Convention.WindowsUI
{

    public class ModernUIImage : WindowUIModule
    {
        [Resources, SerializeField, HopeNotNull] private RawImage m_RawImage;
        [Resources, SerializeField] private GradientEffect m_GradientEffect;

        private void Reset()
        {
            m_RawImage = GetComponent<RawImage>();
        }
        private void Start()
        {
            if (m_RawImage == null)
                m_RawImage = GetComponent<RawImage>();
        }
    }
}
