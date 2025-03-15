using System.Collections;
using System.Collections.Generic;
using Cinemachine;
using UnityEngine;
using UnityEngine.InputSystem;

namespace Convention
{
    public class FreeSceneCamera : MonoAnyBehaviour
    {
        [Resources] public Transform TargetFollow;
        [Resources] public CinemachineVirtualCamera VirtualCamera;
        [Setting] public float moveSpeed = 1;
        [Setting] public float rotationSpeed = 1;
        private bool m_IsFocus = false;
        [Setting]
        public bool isFocus
        {
            get => m_IsFocus;
            set
            {
                if (m_IsFocus != value)
                {
                    m_IsFocus = value;
                    Cursor.lockState = m_IsFocus ? CursorLockMode.Locked : CursorLockMode.None;
                    Cursor.visible = !m_IsFocus;
                }
            }
        }
        [Setting] public Vector2 RotationRoot = new(0, 0);

        private void Update()
        {
            Vector3 dxyz = Vector3.zero;
            Vector3 rxyz = Vector3.zero;
            if (Keyboard.current[Key.W].isPressed || Keyboard.current[Key.UpArrow].isPressed)
                dxyz += transform.forward;
            if (Keyboard.current[Key.A].isPressed || Keyboard.current[Key.LeftArrow].isPressed)
                dxyz += -transform.right;
            if (Keyboard.current[Key.S].isPressed || Keyboard.current[Key.RightArrow].isPressed)
                dxyz += transform.right;
            if (Keyboard.current[Key.S].isPressed || Keyboard.current[Key.DownArrow].isPressed)
                dxyz += -transform.forward;
            if (Keyboard.current[Key.Space].isPressed)
                dxyz += transform.up;
            if (Keyboard.current[Key.LeftShift].isPressed)
                dxyz += -transform.up;

            var drotation = Vector2.zero;
            if (isFocus)
            {
                drotation = Mouse.current.delta.ReadValue();
            }

            //

            TargetFollow.Translate(dxyz * Time.deltaTime, Space.Self);
            TargetFollow.Rotate(drotation * Time.deltaTime, Space.Self);

            //

            if (Keyboard.current[Key.Escape].isPressed)
                isFocus = false;
        }
    }
}
