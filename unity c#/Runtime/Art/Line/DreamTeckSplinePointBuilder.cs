#if DREAMTECK_SPLINES
using System.Collections.Generic;
using Convention.WindowsUI.Variant;
using Dreamteck.Splines;
using UnityEngine;

namespace Convention.VFX
{
    public class DreamTeckSplinePointBuilder : MonoAnyBehaviour, ILoadedInHierarchy
    {
        public enum InjectType
        {
            None = -1,
            SmoothMirrored = SplinePoint.Type.SmoothMirrored,
            Broken = SplinePoint.Type.Broken,
            SmoothFree = SplinePoint.Type.SmoothFree
        };
        [Setting, InspectorDraw] public PerformanceIndicator.PerformanceMode performanceMode = PerformanceIndicator.PerformanceMode.Speed;
        [Content] public List<LinePoint> childPoints = new();

        [Resources, SerializeField, HopeNotNull, InspectorDraw] private SplineComputer m_splineComputer;
        [Setting, InspectorDraw] public InjectType PointType = InjectType.None;
        [Resources, SerializeField, HopeNotNull, InspectorDraw] private SplineRenderer m_splineRenderer;
        public SplineComputer MainSpline => m_splineComputer;
        [Content] public List<SplinePoint> knots = new();

        [InspectorDraw]
        public Vector2 Range
        {
            get => new((float)m_splineRenderer.clipFrom, (float)m_splineRenderer.clipTo);
            set
            {
                m_splineRenderer.SetClipRange(value.x, value.y);
            }
        }
        [Percentage(0, 1), InspectorDraw]
        public float Head
        {
            get => (float)m_splineRenderer.clipFrom;
            set => m_splineRenderer.clipFrom = value;
        }
        [Percentage(0, 1), InspectorDraw]
        public float Tail
        {
            get => (float)m_splineRenderer.clipTo;
            set => m_splineRenderer.clipTo = value;
        }
        [Percentage(0, 1), InspectorDraw]
        public float Duration
        {
            get => (float)(m_splineRenderer.clipTo - m_splineRenderer.clipFrom);
            set => m_splineRenderer.clipTo = m_splineRenderer.clipFrom + value;
        }
        public float Distance
        {
            get => m_splineRenderer.CalculateLength(Head, Tail);
            set
            {
                var t = value / m_splineRenderer.CalculateLength(Head, Tail);
                Duration = t;
            }
        }
        public float GetDistanceBetweenHeadAndBegin()
        {
            return m_splineRenderer.CalculateLength(0, Head);
        }
        public float GetDistanceBetweenHeadAndTail()
        {
            return m_splineRenderer.CalculateLength(Head, Tail);
        }
        public float GetDistanceBetweenTailAndBegin()
        {
            return m_splineRenderer.CalculateLength(0, Tail);
        }
        public float GetDistanceBetweenEndAndTail()
        {
            return m_splineRenderer.CalculateLength(Tail, 1);
        }
        public float GetDistanceBetweenEndAndHead()
        {
            return m_splineRenderer.CalculateLength(Head,1);
        }
        public float GetTotalDistance()
        {
            return m_splineRenderer.CalculateLength(0, 1);
        }


        private void Reset()
        {
            m_splineComputer = GetComponent<SplineComputer>();
            m_splineRenderer = GetComponent<SplineRenderer>();
        }

        void Start()
        {
            if (m_splineComputer == null)
                m_splineComputer = GetComponent<SplineComputer>();
            if (m_splineRenderer == null)
                m_splineRenderer = GetComponent<SplineRenderer>();
        }

        private void LateUpdate()
        {
            if ((int)performanceMode >= (int)PerformanceIndicator.PerformanceMode.L6)
            {
                RebuildAll();
                m_splineRenderer.Rebuild();
            }
            else if ((int)performanceMode >= (int)PerformanceIndicator.PerformanceMode.L6)
            {
                if (childPoints.Count != knots.Count)
                    RebuildAll();
                else
                    ResetPoints();
                m_splineRenderer.Rebuild();
            }
        }

        public static void SetKnot([In, ArgPackage] ref SplinePoint point, [In, ArgPackage] LinePoint linePoint)
        {
            point.position = linePoint.transform.localPosition;
            point.normal = linePoint.Forward;
            point.size = linePoint.Scale;
            point.color = linePoint.PointColor;
        }

        [Content]
        public void RebuildAll()
        {
            if (knots.Count != childPoints.Count)
            {
                for (int i = knots.Count, e = childPoints.Count; i != e; i++)
                    knots.Add(new());
            }
            if (knots.Count != childPoints.Count)
            {
                knots.RemoveRange(childPoints.Count, knots.Count - childPoints.Count);
            }
            MainSpline.SetPoints(knots.ToArray());

            // 更新所有点的位置和类型
            for (int i = 0; i < childPoints.Count; i++)
            {
                var point = knots[i];
                SetKnot(ref point, childPoints[i]);
                if (PointType != InjectType.None)
                    point.type = (SplinePoint.Type)PointType;
                MainSpline.SetPoint(i, knots[i] = point, SplineComputer.Space.Local);
            }
        }
        [Content]
        public void ResetPoints()
        {
            if (knots.Count == 0)
                return;
            for (int i = 0; i < childPoints.Count; i++)
            {
                var point = knots[i];
                SetKnot(ref point, childPoints[i]);
                if (PointType != InjectType.None)
                    point.type = (SplinePoint.Type)PointType;
                MainSpline.SetPoint(i, knots[i] = point);
            }
            for (int i = childPoints.Count, e = knots.Count; i < e; i++)
            {
                var point = knots[i];
                SetKnot(ref point, childPoints[i]);
                if (PointType != InjectType.None)
                    point.type = (SplinePoint.Type)PointType;
                MainSpline.SetPoint(i, knots[i] = point);
            }
        }
        [Content]
        [return: ReturnNotNull]
        public LinePoint AddChild()
        {
            var trans = new GameObject("Point").AddComponent<LinePoint>();
            if (childPoints.Count > 0)
            {
                trans.transform.SetParent(childPoints[^1].transform.parent);
                trans.transform.position = childPoints[^1].transform.position;
            }
            childPoints.Add(trans);
            if (HierarchyWindow.instance && HierarchyWindow.instance.ContainsReference(this))
            {
                var item = HierarchyWindow.instance.GetReferenceItem(this)
                    .CreateSubPropertyItemWithBinders(trans.gameObject)[0];
                item.ref_value.GetComponent<HierarchyItem>().title = trans.gameObject.name;
            }
            RebuildAll();
            return trans;
        }
    }
}
#endif
