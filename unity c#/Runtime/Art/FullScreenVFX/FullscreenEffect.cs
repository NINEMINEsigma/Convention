using System;
using System.Linq;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.Rendering;
#if UNITY_URP
using UnityEngine.Rendering.Universal;

namespace Convention
{
    namespace VFX
    {
        // Empty class to be used in scenes and doesn't implement any additional overrides
        public abstract class FullscreenEffect : FullscreenEffectBase<FullscreenPassBase>
        {
            public abstract void SetEffectWeight([Percentage(0f, 1f)] float value);
        }

        [ExecuteAlways]
        public class FullscreenEffectBase<T> : MonoAnyBehaviour where T : FullscreenPassBase, new()
        {
            [Ignore] private T _pass;

            [Ignore] public Material PassMaterial { get => _pass.material; }

            [SerializeField, Setting]
            private string _passName = "Fullscreen Pass";

            [SerializeField, Setting]
            private Material _material;

            [SerializeField, Setting]
            private RenderPassEvent _injectionPoint = RenderPassEvent.BeforeRenderingTransparents;
            [SerializeField, Setting]
            private int _injectionPointOffset = 0;
            [SerializeField, Setting]
            private ScriptableRenderPassInput _inputRequirements = ScriptableRenderPassInput.None;
            [SerializeField, Setting]
            private CameraType _cameraType = CameraType.Game | CameraType.SceneView;


            private void OnEnable()
            {
                SetupPass();

                RenderPipelineManager.beginCameraRendering += OnBeginCamera;
            }

            private void OnDisable()
            {
                RenderPipelineManager.beginCameraRendering -= OnBeginCamera;
            }

            public virtual void SetupPass()
            {
                _pass ??= new T();

                // pass setup
                _pass.renderPassEvent = _injectionPoint + _injectionPointOffset;
                _pass.material = _material;
                if (_material != null)
                {
                    _pass.hasYFlipKeyword = _material.shader.keywordSpace.keywordNames.Contains("_FLIPY");

                    if (_pass.hasYFlipKeyword)
                        _pass.yFlipKeyword = new LocalKeyword(_material.shader, "_FLIPY");
                }
                _pass.passName = _passName;

                _pass.ConfigureInput(_inputRequirements);
            }

            public virtual void OnBeginCamera(ScriptableRenderContext ctx, Camera cam)
            {
                // Skip if pass wasn't initialized or if material is empty
                if (_pass == null || _material == null)
                    return;

                // Only draw for selected camera types
                if ((cam.cameraType & _cameraType) == 0) return;

                // injection pass
                cam.GetUniversalAdditionalCameraData().scriptableRenderer.EnqueuePass(_pass);
            }

            private void OnValidate()
            {
                SetupPass();
            }
        }

        public class FullscreenPassBase : ScriptableRenderPass
        {
            public Material material;

            public bool hasYFlipKeyword;
            public LocalKeyword yFlipKeyword;
            public string passName = "Fullscreen Pass";

            public UnityAction<Material> additionalExecuteAction;

            public override void Execute(ScriptableRenderContext context, ref RenderingData renderingData)
            {
                if (hasYFlipKeyword)
                    material.SetKeyword(
                        yFlipKeyword,
                        renderingData.cameraData.IsRenderTargetProjectionMatrixFlipped(renderingData.cameraData.renderer.cameraColorTargetHandle)
                        );

                var cmd = CommandBufferPool.Get(passName);

                CoreUtils.DrawFullScreen(cmd, material);

                context.ExecuteCommandBuffer(cmd);
                cmd.Clear();
            }
        }
    }
}
#endif