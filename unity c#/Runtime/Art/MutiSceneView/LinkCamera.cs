using System.Collections;
using System.Collections.Generic;
using Convention.Internal;
using UnityEngine;

namespace Convention
{
    namespace VFX
    {
        namespace MutiSceneView
        {
            public class LinkCamera : MonoAnyBehaviour
            {
                [Resources] public Material LinkMaterial;
                [Content, Ignore, SerializeField, Header("SwitchAnimation")] private float Ticker = 0;
                [Content, Ignore, SerializeField] private bool IsOpen = false;
                [Resources, Header("LEDPatternResolution")] public Transform First;
                [Resources] public Transform Second;
                [Setting,Header("SwitchAnimation")] public float Duration = 1f;
                [Setting] public AnimationCurve TickerCurve = AnimationCurve.Linear(0, 0, 1, 1);
                [Setting, Header("LEDPatternResolution")] public AnimationCurve LEDCurve = AnimationCurve.Linear(1, 100, 5, 500);

                public void LoadTextureView()
                {
                    IsOpen = true;
                }
                public void UnloadTextureView()
                {
                    IsOpen = false;
                }
                private void Update()
                {
                    Ticker = Mathf.Clamp(Ticker + (IsOpen ? 1 : -1) * Time.deltaTime, 0, Duration);
                    LinkMaterial.SetFloat("_SwitchAnimation", TickerCurve.Evaluate(Ticker / Duration));
                    LinkMaterial.SetFloat("_LEDPatternResolution", Mathf.Clamp(LEDCurve.Evaluate((First.position - Second.position).magnitude), 10, 1000));
                }
            }
        }
    }
}
