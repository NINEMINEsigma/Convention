using System.Collections.Generic;
using System.Linq;
using UnityEngine;

namespace Convention.WindowsUI.Variant
{
    public class FocusWindowIndictaor : MonoSingleton<FocusWindowIndictaor>
    {
        [Setting, Range(0, 1), Percentage(0, 1)] public float Speed = 0.36f;
        [Resources, OnlyNotNullMode] public RectTransform RectBox;
        [Resources, OnlyNotNullMode] public RectTransform RopParent;
        [Resources] public List<RectTransform> Targets = new();
        [Content] public int TargetIndex;
        [Content, OnlyPlayMode] public RectTransform Target;

        public void SetTargetRectTransform(RectTransform target)
        {
            Target = target;
        }
        public void SelectNextTarget()
        {
            Debug.Log(TargetIndex);
            Target = Targets[TargetIndex = (TargetIndex + 1) % Targets.Count];
        }

        private void FixedUpdate()
        {
            if (Target != null)
                RectTransformInfo.UpdateAnimationPlane(Target, RectBox, Speed, 0, true);
            else
                RectTransformInfo.UpdateAnimationPlane(RopParent, RectBox, Speed, 0, true);
        }
    }
}
