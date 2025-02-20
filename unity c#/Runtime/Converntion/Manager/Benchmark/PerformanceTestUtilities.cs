using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UIElements;

namespace Convention
{
    namespace Benchmarking
    {
        public static class PerformanceTestUtilities
        {
            public static void Set(this Label label, FrameData frameData)
            {
                label.text = frameData.GetValueString(PerformanceTestManager.displayedDataType);
                label.style.color = PerformanceTestManager.GetColorForFrameData(frameData);
            }
        }
    }
}