using System.Collections;
using System.Collections.Generic;
using Convention.WindowsUI.Variant;
using UnityEngine;

namespace Convention
{
    public class WorkflowManager : MonoSingleton<WorkflowManager>
    {
        private RegisterWrapper<WorkflowManager> m_RegisterWrapper;

        private void Start()
        {
            m_RegisterWrapper = new(() =>
            {

            }, typeof(HierarchyWindow), typeof(InspectorWindow));
        }
    }
}
