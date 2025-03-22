using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Convention.VFX
{
    [RequireComponent(typeof(LineRenderer))]
    public class LinePointContainer : MonoAnyBehaviour
    {
        [Resources, HopeNotNull, SerializeField] private LineRenderer lineRenderer;
        [Setting] public PerformanceIndicator.PerformanceMode performanceMode = PerformanceIndicator.PerformanceMode.Quality;
        private void Reset()
        {
            lineRenderer = GetComponent<LineRenderer>();
        }

        private void Start()
        {
            if (lineRenderer == null)
                lineRenderer = GetComponent<LineRenderer>();
        }

        private void LateUpdate()
        {
            if((int)performanceMode>(int)PerformanceIndicator.PerformanceMode.L6)
            {
                Rebuild();
            }
        }

        public void Rebuild()
        {
            
        }
    }
}
