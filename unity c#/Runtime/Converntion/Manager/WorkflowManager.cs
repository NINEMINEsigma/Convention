using System.Collections;
using System.Collections.Generic;
using Convention.WindowsUI.Variant;
using Convention.Workflow;
using UnityEngine;
using UnityEngine.InputSystem;

namespace Convention.Workflow
{
    public class WorkflowManager : MonoSingleton<WorkflowManager>
    {
        private RegisterWrapper<WorkflowManager> m_RegisterWrapper;
        [Resources, OnlyNotNullMode, SerializeField] private Camera m_MainCamera;
        [Setting] public float ScrollSpeed = 1;

        private void Start()
        {
            m_RegisterWrapper = new(() =>
            {

            }, typeof(GraphInputWindow));
        }

        private void Update()
        {
            if (Keyboard.current[Key.LeftCtrl].isPressed)
            {
                var t = Mouse.current.scroll.y.ReadValue() * ScrollSpeed * 0.001f;
                var z = m_MainCamera.transform.localPosition.z;
                if (z - t > -100 && z - t < -5)
                    m_MainCamera.transform.Translate(new Vector3(0, 0, -t), Space.Self);
            }
        }
    }
}
