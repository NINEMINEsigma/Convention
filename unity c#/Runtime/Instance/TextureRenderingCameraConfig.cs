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

            private void Reset()
            {
                m_RenderTextureScaleName = "RenderTextureScale";
                this.Datas[m_RenderTextureScaleName] = new(1f);
            }

            private void OnValidate()
            {
                if(this.Datas.ContainsKey(m_RenderTextureScaleName)==false)
                {
                    Reset();
                }
            }

            public override void Invoke(Camera camera)
            {
                camera.targetTexture = new RenderTexture(
                    (int)(camera.scaledPixelWidth *this.Datas[m_RenderTextureScaleName].floatValue),
                    (int)(camera.scaledPixelHeight * this.Datas[m_RenderTextureScaleName].floatValue),
                    GraphicsFormat.R16G16B16A16_SFloat, GraphicsFormat.D24_UNorm_S8_UInt
                    );
            }
        }
    }
}
