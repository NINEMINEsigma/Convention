using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Convention.WindowsUI.Variant;
using UnityEngine;
#if DREAMTECK_SPLINES
using Dreamteck.Splines;
#else
using UnityEngine.Splines;
#endif

namespace Convention.VFX
{
    [ExecuteAlways]
    public class SplinePointBuilder : MonoAnyBehaviour, ILoadedInHierarchy
    {
        [Setting, InspectorDraw] public PerformanceIndicator.PerformanceMode performanceMode = PerformanceIndicator.PerformanceMode.Speed;
        [Content] public List<LinePoint> childPoints = new();

#if DREAMTECK_SPLINES
        [Resources, SerializeField, HopeNotNull, InspectorDraw] private SplineComputer m_splineContainer;
        public SplineComputer MainSpline => m_splineContainer;
#else
        [Resources, SerializeField, HopeNotNull, InspectorDraw] private SplineContainer m_splineContainer;
        public Spline MainSpline => m_splineContainer.Spline;
        [Resources, SerializeField, HopeNotNull, InspectorDraw] private SplineExtrude m_splineExtrude;
        [Content] public List<BezierKnot> bezierKnots = new List<BezierKnot>();
#endif

        [InspectorDraw]
        public Vector2 Range
        {
            get => new();//m_splineExtrude.Range;
            set { }// m_splineExtrude.Range = value;
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
#if DREAMTECK_SPLINES

#else
            m_splineExtrude = GetComponent<SplineExtrude>();
            m_splineContainer = GetComponent<SplineContainer>();
            if (m_splineExtrude != null)
                m_splineExtrude.Container = m_splineContainer;
#endif
        }

        void Start()
        {
#if DREAMTECK_SPLINES

#else
            if (m_splineExtrude == null)
                m_splineExtrude = GetComponent<SplineExtrude>();
            if (m_splineContainer == null)
            {
                m_splineContainer = GetComponent<SplineContainer>();
                m_splineExtrude.Container = m_splineContainer;
            }
#endif
        }

        private void LateUpdate()
        {
            if ((int)performanceMode >= (int)PerformanceIndicator.PerformanceMode.L8)
            {
                RebuildAll();
            }
            else if ((int)performanceMode >= (int)PerformanceIndicator.PerformanceMode.L6)
            {
                ResetPoints();
            }
        }
#if DREAMTECK_SPLINES

#else
        [Setting, SerializeField] private BezierKnot BezierKnotTemplate = new();
        public BezierKnot BuildPoint(Vector3 position)
        {
            return new()
            {
                Position = position,
                Rotation = BezierKnotTemplate.Rotation
            };
        }
#endif

        [Content]
        public void RebuildAll()
        {
#if DREAMTECK_SPLINES

#else
            int lastcount = bezierKnots.Count;
            if (bezierKnots.Count < childPoints.Count)
            {
                bezierKnots.AddRange(new BezierKnot[childPoints.Count - bezierKnots.Count]);
            }
            else if (bezierKnots.Count == childPoints.Count)
            {

            }
            else
            {
                bezierKnots = new BezierKnot[childPoints.Count].ToList();
            }
            MainSpline.Knots = bezierKnots;
            for (int i = 0, e = childPoints.Count; i < e; i++)
            {
                MainSpline.SetKnot(i, new BezierKnot(childPoints[i].transform.position));
            }
            for (int i = childPoints.Count, e = lastcount; i < e; i++)
            {
                MainSpline.RemoveAt(i);
            }
#endif
        }
        [Content]
        public void ResetPoints()
        {
            for (int i = 0, e = childPoints.Count; i < e; i++)
            {
#if DREAMTECK_SPLINES

#else
                MainSpline.SetKnot(i, new BezierKnot(childPoints[i].transform.position));
#endif
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
 