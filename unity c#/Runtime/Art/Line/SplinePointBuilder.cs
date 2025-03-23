#if UNITY_SPLINE
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Convention.WindowsUI.Variant;
using UnityEngine;
using UnityEngine.Splines;

namespace Convention.VFX
{
    public class SplinePointBuilder : MonoAnyBehaviour, ILoadedInHierarchy
    {
        [Setting, InspectorDraw] public PerformanceIndicator.PerformanceMode performanceMode = PerformanceIndicator.PerformanceMode.Speed;
        [Content] public List<LinePoint> childPoints = new();

        [Resources, SerializeField, HopeNotNull, InspectorDraw] private SplineContainer m_splineContainer;
        public Spline MainSpline => m_splineContainer.Spline;
        [Resources, SerializeField, HopeNotNull, InspectorDraw] private SplineExtrude m_splineExtrude;
        [Content] public List<BezierKnot> knots = new List<BezierKnot>();

        [InspectorDraw]
        public Vector2 Range
        {
            get => m_splineExtrude.Range;
            set => m_splineExtrude.Range = value;
        }
        [Percentage(0, 1), InspectorDraw]
        public float Head
        {
            get => Range.x;
            set => Range = new(value, Range.y);
        }
        [Percentage(0, 1), InspectorDraw]
        public float Tail
        {
            get => Range.y;
            set => Range = new(Range.x, value);
        }
        [Percentage(0, 1), InspectorDraw]
        public float Duration
        {
            get => Range.y - Range.x;
            set => Tail = Head + value;
        }


        private void Reset()
        {
            m_splineExtrude = GetComponent<SplineExtrude>();
            m_splineContainer = GetComponent<SplineContainer>();
            if (m_splineExtrude != null)
                m_splineExtrude.Container = m_splineContainer;
        }

        void Start()
        {
            if (m_splineExtrude == null)
                m_splineExtrude = GetComponent<SplineExtrude>();
            if (m_splineContainer == null)
            {
                m_splineContainer = GetComponent<SplineContainer>();
                m_splineExtrude.Container = m_splineContainer;
            }
        }

        private void LateUpdate()
        {
            if ((int)performanceMode >= (int)PerformanceIndicator.PerformanceMode.L8)
            {
                if (childPoints.Count != knots.Count)
                    RebuildAll();
                else
                    ResetPoints();
                m_splineExtrude.Rebuild();
            }
        }

        [Content]
        public void RebuildAll()
        {
            int lastcount = knots.Count;
            if (knots.Count < childPoints.Count)
            {
                knots.AddRange(new BezierKnot[childPoints.Count - knots.Count]);
            }
            else if (knots.Count == childPoints.Count)
            {

            }
            else
            {
                knots = new BezierKnot[childPoints.Count].ToList();
            }
            MainSpline.Knots = knots;
            for (int i = 0, e = childPoints.Count; i < e; i++)
            {
                MainSpline.SetKnot(i, new BezierKnot(childPoints[i].transform.localPosition));
            }
            for (int i = childPoints.Count, e = lastcount; i < e; i++)
            {
                MainSpline.RemoveAt(i);
            }
        }
        [Content]
        public void ResetPoints()
        {
            for (int i = 0, e = childPoints.Count; i < e; i++)
            {
                MainSpline.SetKnot(i, knots[i] = new BezierKnot(childPoints[i].transform.localPosition));
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
#else


#endif