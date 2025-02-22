using System;
using UnityEngine;

namespace Convention
{
    namespace SO
    {
        [CreateAssetMenu(fileName = "new EaseCurve", menuName = "Convention/EaseCurve", order = -1)]
        public class EaseCurve : ScriptableObject
        {
            [Setting] public Convention.MathExtension.EaseCurveType CurveType;

            public float Evaluate(float t)
            {
                return Convention.MathExtension.Evaluate(t, CurveType);
            }
            public float Evaluate(float from, float to, float t)
            {
                return (to - from) * Convention.MathExtension.Evaluate(t, CurveType) + from;
            }
        }
    }
}
