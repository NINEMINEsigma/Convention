using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
#if UNITY_URP
using static Unity.Mathematics.math;
#endif
using static Convention.MathExtension;

namespace Convention
{
    public static partial class MathExtension
    {
        #region EaseCurve

        public static AnimationCurve MakeAnimationCurve(EaseCurveType type, float start, float end, float startValue, float endValue, int keyCount)
        {
            var curve = new AnimationCurve();
            curve.keys = new Keyframe[keyCount];
            for (int i = 0; i < keyCount; i++)
            {
                float t = i / (float)(keyCount - 1);
                curve.keys[i] = new Keyframe(
                    Mathf.Lerp(start, end, t),
                    Mathf.Lerp(startValue, endValue, Evaluate(t, type))
                    );
            }
            return curve;
        }
        public enum EaseCurveType
        {
            Linear = 0,
            InQuad = 1,
            OutQuad = 2,
            InOutQuad = 3,
            InCubic = 4,
            OutCubic = 5,
            InOutCubic = 6,
            InQuart = 7,
            OutQuart = 8,
            InOutQuart = 9,
            InQuint = 10,
            OutQuint = 11,
            InOutQuint = 12,
            InSine = 13,
            OutSine = 14,
            InOutSine = 15,
            InExpo = 16,
            OutExpo = 17,
            InOutExpo = 18,
            InCirc = 19,
            OutCirc = 20,
            InOutCirc = 21,
            InBounce = 22,
            OutBounce = 23,
            InOutBounce = 24,
            InElastic = 25,
            OutElastic = 26,
            InOutElastic = 27,
            InBack = 28,
            OutBack = 29,
            InOutBack = 30,
            Custom = 31
        }

        public static float Evaluate(float t, EaseCurveType curveType)
        {
            float from = 0;
            float to = 1;

            return curveType switch
            {
                EaseCurveType.Linear => Linear(from, to, t),
                EaseCurveType.InQuad => InQuad(from, to, t),
                EaseCurveType.OutQuad => OutQuad(from, to, t),
                EaseCurveType.InOutQuad => InOutQuad(from, to, t),
                EaseCurveType.InCubic => InCubic(from, to, t),
                EaseCurveType.OutCubic => OutCubic(from, to, t),
                EaseCurveType.InOutCubic => InOutCubic(from, to, t),
                EaseCurveType.InQuart => InQuart(from, to, t),
                EaseCurveType.OutQuart => OutQuart(from, to, t),
                EaseCurveType.InOutQuart => InOutQuart(from, to, t),
                EaseCurveType.InQuint => InQuint(from, to, t),
                EaseCurveType.OutQuint => OutQuint(from, to, t),
                EaseCurveType.InOutQuint => InOutQuint(from, to, t),
                EaseCurveType.InSine => InSine(from, to, t),
                EaseCurveType.OutSine => OutSine(from, to, t),
                EaseCurveType.InOutSine => InOutSine(from, to, t),
                EaseCurveType.InExpo => InExpo(from, to, t),
                EaseCurveType.OutExpo => OutExpo(from, to, t),
                EaseCurveType.InOutExpo => InOutExpo(from, to, t),
                EaseCurveType.InCirc => InCirc(from, to, t),
                EaseCurveType.OutCirc => OutCirc(from, to, t),
                EaseCurveType.InOutCirc => InOutCirc(from, to, t),
                EaseCurveType.InBounce => InBounce(from, to, t),
                EaseCurveType.OutBounce => OutBounce(from, to, t),
                EaseCurveType.InOutBounce => InOutBounce(from, to, t),
                EaseCurveType.InElastic => InElastic(from, to, t),
                EaseCurveType.OutElastic => OutElastic(from, to, t),
                EaseCurveType.InOutElastic => InOutElastic(from, to, t),
                EaseCurveType.InBack => InBack(from, to, t),
                EaseCurveType.OutBack => OutBack(from, to, t),
                EaseCurveType.InOutBack => InOutBack(from, to, t),
                _ => throw new NotImplementedException()
            };
        }

        public static float Linear(float from, float to, float t)
        {
            float c = to - from;
            t /= 1f;
            return c * t / 1f + from;
        }

        public static float InQuad(float from, float to, float t)
        {
            float c = to - from;
            t /= 1f;
            return c * t * t + from;
        }

        public static float OutQuad(float from, float to, float t)
        {
            float c = to - from;
            t /= 1f;
            return -c * t * (t - 2f) + from;
        }

        public static float InOutQuad(float from, float to, float t)
        {
            float c = to - from;
            t /= 0.5f;
            if (t < 1) return c / 2f * t * t + from;
            t--;
            return -c / 2f * (t * (t - 2) - 1) + from;
        }

        public static float InCubic(float from, float to, float t)
        {
            float c = to - from;
            t /= 1f;
            return c * t * t * t + from;
        }

        public static float OutCubic(float from, float to, float t)
        {
            float c = to - from;
            t /= 1f;
            t--;
            return c * (t * t * t + 1) + from;
        }

        public static float InOutCubic(float from, float to, float t)
        {
            float c = to - from;
            t /= 0.5f;
            if (t < 1) return c / 2f * t * t * t + from;
            t -= 2;
            return c / 2f * (t * t * t + 2) + from;
        }

        public static float InQuart(float from, float to, float t)
        {
            float c = to - from;
            t /= 1f;
            return c * t * t * t * t + from;
        }

        public static float OutQuart(float from, float to, float t)
        {
            float c = to - from;
            t /= 1f;
            t--;
            return -c * (t * t * t * t - 1) + from;
        }

        public static float InOutQuart(float from, float to, float t)
        {
            float c = to - from;
            t /= 0.5f;
            if (t < 1) return c / 2f * t * t * t * t + from;
            t -= 2;
            return -c / 2f * (t * t * t * t - 2) + from;
        }

        public static float InQuint(float from, float to, float t)
        {
            float c = to - from;
            t /= 1f;
            return c * t * t * t * t * t + from;
        }

        public static float OutQuint(float from, float to, float t)
        {
            float c = to - from;
            t /= 1f;
            t--;
            return c * (t * t * t * t * t + 1) + from;
        }

        public static float InOutQuint(float from, float to, float t)
        {
            float c = to - from;
            t /= 0.5f;
            if (t < 1) return c / 2f * t * t * t * t * t + from;
            t -= 2;
            return c / 2f * (t * t * t * t * t + 2) + from;
        }

        public static float InSine(float from, float to, float t)
        {
            float c = to - from;
            return -c * Mathf.Cos(t / 1f * (Mathf.PI / 2f)) + c + from;
        }

        public static float OutSine(float from, float to, float t)
        {
            float c = to - from;
            return c * Mathf.Sin(t / 1f * (Mathf.PI / 2f)) + from;
        }

        public static float InOutSine(float from, float to, float t)
        {
            float c = to - from;
            return -c / 2f * (Mathf.Cos(Mathf.PI * t / 1f) - 1) + from;
        }

        public static float InExpo(float from, float to, float t)
        {
            float c = to - from;
            return c * Mathf.Pow(2, 10 * (t / 1f - 1)) + from;
        }

        public static float OutExpo(float from, float to, float t)
        {
            float c = to - from;
            return c * (-Mathf.Pow(2, -10 * t / 1f) + 1) + from;
        }

        public static float InOutExpo(float from, float to, float t)
        {
            float c = to - from;
            t /= 0.5f;
            if (t < 1f) return c / 2f * Mathf.Pow(2, 10 * (t - 1)) + from;
            t--;
            return c / 2f * (-Mathf.Pow(2, -10 * t) + 2) + from;
        }

        public static float InCirc(float from, float to, float t)
        {
            float c = to - from;
            t /= 1f;
            return -c * (Mathf.Sqrt(1 - t * t) - 1) + from;
        }

        public static float OutCirc(float from, float to, float t)
        {
            float c = to - from;
            t /= 1f;
            t--;
            return c * Mathf.Sqrt(1 - t * t) + from;
        }

        public static float InOutCirc(float from, float to, float t)
        {
            float c = to - from;
            t /= 0.5f;
            if (t < 1) return -c / 2f * (Mathf.Sqrt(1 - t * t) - 1) + from;
            t -= 2;
            return c / 2f * (Mathf.Sqrt(1 - t * t) + 1) + from;
        }

        public static float InBounce(float from, float to, float t)
        {
            float c = to - from;
            return c - OutBounce(0f, c, 1f - t) + from; //does this work?
        }

        public static float OutBounce(float from, float to, float t)
        {
            float c = to - from;

            if ((t /= 1f) < (1 / 2.75f))
            {
                return c * (7.5625f * t * t) + from;
            }
            else if (t < (2 / 2.75f))
            {
                return c * (7.5625f * (t -= (1.5f / 2.75f)) * t + .75f) + from;
            }
            else if (t < (2.5 / 2.75))
            {
                return c * (7.5625f * (t -= (2.25f / 2.75f)) * t + .9375f) + from;
            }
            else
            {
                return c * (7.5625f * (t -= (2.625f / 2.75f)) * t + .984375f) + from;
            }
        }

        public static float InOutBounce(float from, float to, float t)
        {
            float c = to - from;
            if (t < 0.5f) return InBounce(0, c, t * 2f) * 0.5f + from;
            return OutBounce(0, c, t * 2 - 1) * 0.5f + c * 0.5f + from;

        }

        public static float InElastic(float from, float to, float t)
        {
            float c = to - from;
            if (t == 0) return from;
            if ((t /= 1f) == 1) return from + c;
            float p = 0.3f;
            float s = p / 4f;
            return -(c * Mathf.Pow(2, 10 * (t -= 1)) * Mathf.Sin((t - s) * (2 * Mathf.PI) / p)) + from;
        }

        public static float OutElastic(float from, float to, float t)
        {
            float c = to - from;
            if (t == 0) return from;
            if ((t /= 1f) == 1) return from + c;
            float p = 0.3f;
            float s = p / 4f;
            return (c * Mathf.Pow(2, -10 * t) * Mathf.Sin((t - s) * (2 * Mathf.PI) / p) + c + from);
        }

        public static float InOutElastic(float from, float to, float t)
        {
            float c = to - from;
            if (t == 0) return from;
            if ((t /= 0.5f) == 2) return from + c;
            float p = 0.3f * 1.5f;
            float s = p / 4f;
            if (t < 1)
                return -0.5f * (c * Mathf.Pow(2, 10 * (t -= 1f)) * Mathf.Sin((t - 2) * (2 * Mathf.PI) / p)) + from;
            return c * Mathf.Pow(2, -10 * (t -= 1)) * Mathf.Sin((t - s) * (2f * Mathf.PI) / p) * 0.5f + c + from;
        }

        public static float InBack(float from, float to, float t)
        {
            float c = to - from;
            float s = 1.70158f;
            t /= 0.5f;
            return c * t * t * ((s + 1) * t - s) + from;
        }

        public static float OutBack(float from, float to, float t)
        {
            float c = to - from;
            float s = 1.70158f;
            t = t / 1f - 1f;
            return c * (t * t * ((s + 1) * t + s) + 1) + from;
        }

        public static float InOutBack(float from, float to, float t)
        {
            float c = to - from;
            float s = 1.70158f;
            t /= 0.5f;
            if (t < 1) return c / 2f * (t * t * (((s *= (1.525f)) + 1) * t - s)) + from;
            t -= 2;
            return c / 2f * (t * t * (((s *= (1.525f)) + 1) * t + s) + 2) + from;
        }

        #endregion

        #region DataFilter

        public interface IBaseFilter<_Type>
        {
            _Type[][] Filtrate(_Type[][] source, int width, int length);
            void FiltrateSource(_Type[][] source, int width, int length);
        }

        public interface ITreeFilter<_Type, _NextFilter> : IBaseFilter<_Type> where _NextFilter : IBaseFilter<_Type>
        {
            _Type[][] Filtrate(_Type[][] source, int width, int length, _NextFilter next);
        }

        public abstract class AbstractFilter : IBaseFilter<float>
        {
            protected abstract void DoFiltrate(float[][] source, float[][] result, int width, int length);

            public float[][] Filtrate(float[][] source, int width, int length)
            {
                float[][] result = new float[width][];
                for (int i = 0; i < width; i++)
                {
                    result[i] = new float[length];
                }
                DoFiltrate(source, result, width, length);
                return result;
            }

            public void FiltrateSource(float[][] source, int width, int length)
            {
                DoFiltrate(source, source, width, length);
            }
        }

        public abstract class AbstractColorFilter : IBaseFilter<Color>
        {
            protected abstract void DoFiltrate(Color[][] source, Color[][] result, int width, int length);

            public Color[][] Filtrate(Color[][] source, int width, int length)
            {
                Color[][] result = new Color[width][];
                for (int i = 0; i < width; i++)
                {
                    result[i] = new Color[length];
                }
                DoFiltrate(source, result, width, length);
                return result;
            }

            public void FiltrateSource(Color[][] source, int width, int length)
            {
                DoFiltrate(source, source, width, length);
            }
        }

        [Serializable]
        public class TemplateLimitFilter : AbstractFilter
        {
            public float min;
            public float max;
            public float maxEnhance;
            public float maxWeakened;
            public int bufferSize;

            private float[][] buffer;
            private int header = 0;

            public TemplateLimitFilter(float min, float max, float maxEnhance, float maxWeakened, int bufferSize)
            {
                this.min = min;
                this.max = max;
                this.maxEnhance = maxEnhance;
                this.maxWeakened = maxWeakened;
                this.bufferSize = bufferSize;
            }

            protected override void DoFiltrate(float[][] source, float[][] result, int width, int length)
            {
                InitBuffer(source, result, width, length);
                for (int i = 0; i < width; i++)
                {
                    ReadLine(source, result, i, width, length);
                    BuildLine(source, result, i, width, length);
                }
                ClearBuffer();
            }

            protected virtual void InitBuffer(float[][] source, float[][] result, int width, int length)
            {
                buffer ??= new float[bufferSize][];
                for (int i = 0; i < buffer.Length; i++)
                {
                    buffer[i] = new float[length];
                    if (i < width)
                        for (int j = 0; j < length; j++)
                        {
                            buffer[i][j] = source[i][j];
                        }
                }
            }

            protected virtual void ClearBuffer()
            {
                for (int i = 0; i < buffer.Length; i++)
                {
                    buffer[i] = null;
                }
            }

            protected virtual void ReadLine(float[][] source, float[][] result, int offset, int width, int length)
            {
                //Move data to buffer
                for (int i = 0, e = length; i < e; i++)
                {
                    buffer[header][i] = source[offset][i];
                }
                //Push buffer header
                header = (header + 1) % bufferSize;
            }

            protected virtual void BuildLine(float[][] source, float[][] result, int offset, int width, int length)
            {
                int tempHeader = header;
                float[] line = new float[length];
                do
                {
                    //Mark data
                    for (int i = 0; i < length; i++)
                    {
                        float value = buffer[tempHeader][i];
                        int j = (tempHeader - 1) % bufferSize;
                        if (j < 0)
                            j = bufferSize + j;
                        float lastValue = buffer[j][i];
                        line[i] = Mathf.Clamp(Mathf.Clamp(value, lastValue - maxWeakened, lastValue + maxEnhance), min, max);
                    }
                    //Push buffer header
                    tempHeader = (tempHeader + 1) % bufferSize;
                }
                while (tempHeader != header);
                for (int i = 0; i < length; i++)
                {
                    result[offset][i] = line[i];
                }
            }
        }

        [Serializable]
        public class TemplateColorLimitFilter : AbstractColorFilter
        {
            public Color min;
            public Color max;
            public Color maxEnhance;
            public Color maxWeakened;

            public TemplateColorLimitFilter(Color min, Color max, Color maxEnhance, Color maxWeakened)
            {
                this.min = min;
                this.max = max;
                this.maxEnhance = maxEnhance;
                this.maxWeakened = maxWeakened;
            }

            protected override void DoFiltrate(Color[][] source, Color[][] result, int width, int length)
            {
                for (int i = 1; i < width; i++)
                {
                    BuildLine(source, result, i, width, length);
                }
            }

            protected virtual void BuildLine(Color[][] source, Color[][] result, int offset, int width, int length)
            {
                Color[] line = new Color[length];
                //Mark data
                for (int i = 0; i < length; i++)
                {
                    int j = (offset - 1) % source.Length;
                    Color value = source[j][i];
                    Color lastValue = source[offset][i];
                    line[i] = new(BuildColorR(value, lastValue),
                                  BuildColorG(value, lastValue),
                                  BuildColorB(value, lastValue),
                                  BuildColorA(value, lastValue));
                }
                for (int i = 0; i < length; i++)
                {
                    result[offset][i] = line[i];
                }
            }

            protected float BuildColorA(Color value, Color lastValue)
            {
                return Mathf.Clamp(Mathf.Clamp(value.a, lastValue.a - maxWeakened.a, lastValue.a + maxEnhance.a), min.a, max.a);
            }

            protected float BuildColorR(Color value, Color lastValue)
            {
                return Mathf.Clamp(Mathf.Clamp(value.r, lastValue.r - maxWeakened.r, lastValue.r + maxEnhance.r), min.r, max.r);
            }

            protected float BuildColorG(Color value, Color lastValue)
            {
                return Mathf.Clamp(Mathf.Clamp(value.g, lastValue.g - maxWeakened.g, lastValue.g + maxEnhance.g), min.g, max.g);
            }

            protected float BuildColorB(Color value, Color lastValue)
            {
                return Mathf.Clamp(Mathf.Clamp(value.b, lastValue.b - maxWeakened.b, lastValue.b + maxEnhance.b), min.b, max.b);
            }
        }

        #endregion
    }
}

