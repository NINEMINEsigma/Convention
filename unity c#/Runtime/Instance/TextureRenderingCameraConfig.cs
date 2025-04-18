using UnityEngine;
using UnityEngine.Experimental.Rendering;

namespace Convention
{
    namespace SO
    {
        [CreateAssetMenu(fileName = "new TextureRenderingConfig", menuName = "Convention/Camera/TextureRenderingConfig", order = 0)]
        public class TextureRenderingCameraConfig : CameraInitializerConfig
        {
            [Tooltip("Value Name"), Setting] private string m_RenderTextureScaleName = "RenderTextureScale";

            private void OnEnable()
            {
                Reset();
            }

            public override void Reset()
            {
                base.Reset();
                m_RenderTextureScaleName = "RenderTextureScale";
                this.values[m_RenderTextureScaleName] = 1f;
            }

            private void OnValidate()
            {
                if (this.values.ContainsKey(m_RenderTextureScaleName) == false)
                {
                    Reset();
                }
            }

            public override void Invoke(Camera camera)
            {
                camera.targetTexture = new RenderTexture(
                    (int)(camera.scaledPixelWidth * this.values[m_RenderTextureScaleName]),
                    (int)(camera.scaledPixelHeight * this.values[m_RenderTextureScaleName]),
                    GraphicsFormat.R16G16B16A16_SFloat, GraphicsFormat.D24_UNorm_S8_UInt
                    );
            }
        }
    }
}
