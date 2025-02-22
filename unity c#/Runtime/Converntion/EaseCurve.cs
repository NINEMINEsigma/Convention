using System;

namespace Convention
{
    namespace MathE
    {
        [System.Serializable]
        public class EaseCurve : AnyClass
        {
            [Setting] public Convention.MathExtension.EaseCurveType CurveType;
            public EaseCurve(Convention.MathExtension.EaseCurveType animationCurveType = Convention.MathExtension.EaseCurveType.Linear)
            {
                this.CurveType = animationCurveType;
            }

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
