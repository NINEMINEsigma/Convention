using System.Collections;
using UnityEngine;

namespace Convention
{
    namespace VFX
    {
        namespace MutiSceneView
        {
            public class CameraView : MonoAnyBehaviour
            {
                [Resources,Header("Use \"MutiSceneView\" Shader to generate material")] public Material LinkMaterial;
                [Resources, Header("LEDPatternResolution")] public Transform First;
                [Resources] public Transform Second;
                [Setting,Header("SwitchAnimation")] public float Duration = 1f;
                [Setting] public AnimationCurve TickerCurve = AnimationCurve.Linear(0, 0, 1, 1);
                [Setting, Header("LEDPatternResolution")] public AnimationCurve LEDCurve = AnimationCurve.Linear(1, 100, 5, 500);
                [Content, OnlyPlayMode, Ignore] public bool IsDisplayAnimation = false;

                private void OnEnable()
                {
                    if (First == null)
                        First = Camera.main.transform;
                    if(Second==null)
                        Second = gameObject.transform;
                }

                private IEnumerator DoLoadTexture()
                {
                    IsDisplayAnimation = true;
                    float Ticker = 0;
                    while (Ticker < Duration)
                    {
                        Ticker += Time.deltaTime;
                        LinkMaterial.SetFloat("_SwitchAnimation", TickerCurve.Evaluate(Ticker / Duration));
                        yield return null;
                    }
                    LinkMaterial.SetFloat("_SwitchAnimation", TickerCurve.Evaluate(1));
                    IsDisplayAnimation = false;
                }
                private IEnumerator DoUnloadTexture()
                {
                    IsDisplayAnimation = true;
                    float Ticker = Duration;
                    while (Ticker < Duration)
                    {
                        Ticker -= Time.deltaTime;
                        LinkMaterial.SetFloat("_SwitchAnimation", TickerCurve.Evaluate(Ticker / Duration));
                        yield return null;
                    }
                    LinkMaterial.SetFloat("_SwitchAnimation", TickerCurve.Evaluate(0));
                    IsDisplayAnimation = false;
                }
                public void LoadTextureView()
                {
                    StopCoroutine(nameof(DoLoadTexture));
                    StartCoroutine(DoLoadTexture());
                }
                public void LoadTextureView(RenderTexture texture)
                {
                    LinkMaterial.SetTexture("_ScreenColor", texture);
                    StopCoroutine(nameof(DoLoadTexture));
                    StartCoroutine(DoLoadTexture());
                }
                public void LoadTextureView(Camera camera)
                {
                    LinkMaterial.SetTexture("_ScreenColor", camera.activeTexture);
                    StopCoroutine(nameof(DoLoadTexture));
                    StartCoroutine(DoLoadTexture());
                }
                public void UnloadTextureView()
                {
                    StopCoroutine(nameof(DoUnloadTexture));
                    StartCoroutine(DoUnloadTexture());
                }
                private void Update()
                {
                    LinkMaterial.SetFloat("_LEDPatternResolution", Mathf.Clamp(LEDCurve.Evaluate((First.position - Second.position).magnitude), 10, 1000));
                }
            }
        }
    }
}
