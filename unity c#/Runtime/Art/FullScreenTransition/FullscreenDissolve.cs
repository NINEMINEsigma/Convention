using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering;

namespace Convention
{
    namespace VFX
    {
#if UNITY_URP
        public class FullscreenDissolve : FullscreenEffect
        {
            [Setting, SerializeField] private List<string> IgnoreCameraTag = new();
            [Setting, SerializeField, Tooltip("PassMaterial.TransitionAmount")]
            private string TransitionAmount_Float = "_TransitionAmount";
            [Content, Setting] private bool DissolveNeeded = false;

            [Setting, Header("Dissolve Setting")] public AnimationCurve DissolveZoomInCurve = AnimationCurve.Linear(0, 0, 1, 1);
            [Setting] public AnimationCurve DissolveZoomOutCurve = AnimationCurve.Linear(0, 0, -1, 1);
            [Setting] public float ZommInDuration = 0.5f;
            [Setting] public float ZommOutDuration = 0.5f;
            [Setting] public float StayDuration = 0.5f;

            public override void SetEffectWeight([Percentage(0, 1)] float value)
            {
                this.PassMaterial.SetFloat(TransitionAmount_Float, value);
            }
            private IEnumerator Dissolving(Action midCallback, Action endCallback)
            {
                DissolveNeeded = true;
                float ticks = ZommInDuration;
                while (ticks > 0)
                {
                    SetEffectWeight(DissolveZoomInCurve.Evaluate(1.0f - ticks / ZommInDuration));
                    ticks -= Time.deltaTime;
                    yield return null;
                }
                SetEffectWeight(1);
                midCallback();
                ticks = StayDuration;
                while (ticks > 0)
                {
                    ticks -= Time.deltaTime;
                    yield return null;
                }
                while (ticks < ZommOutDuration)
                {
                    SetEffectWeight(DissolveZoomOutCurve.Evaluate(1.0f - ticks / ZommOutDuration));
                    ticks += Time.deltaTime;
                    yield return null;
                }
                SetEffectWeight(0);
                endCallback();
                DissolveNeeded = false;
            }

            public void StartDissolve()
            {
                StopCoroutine("Dissolving");
                StartCoroutine(Dissolving(() => { }, () => { }));
            }
            public void StartDissolve(Action callback, bool isMid)
            {
                StopCoroutine("Dissolving");
                if (isMid)
                    StartCoroutine(Dissolving(callback, () => { }));
                else
                    StartCoroutine(Dissolving(() => { }, callback));
            }
            public void StartDissolve(Action midCallback, Action endCallback)
            {
                StopCoroutine("Dissolving");
                StartCoroutine(Dissolving(midCallback, endCallback));
            }

            public override void OnBeginCamera(ScriptableRenderContext ctx, Camera cam)
            {
                foreach (var tag in IgnoreCameraTag)
                {
                    if (cam.CompareTag(tag))
                        return;
                }
                if (!DissolveNeeded)
                {
                    return;
                }

                base.OnBeginCamera(ctx, cam);
            }
        }
#endif
    }
}
