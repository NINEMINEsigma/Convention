using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

namespace Convention
{
    public class FPSController : MonoAnyBehaviour
    {
        [Setting] public bool useCharacterForward = false;
        [Setting] public bool lockToCameraForward = false;
        [Setting] public float turnSpeed = 10f;
        [Setting] public KeyCode sprintJoystick = KeyCode.JoystickButton2;
        [Setting] public KeyCode sprintKeyboard = KeyCode.Space;

        [Content, Ignore] private float turnSpeedMultiplier;
        [Setting] public Vector2 turnSpeedMultiplierBoundary = new(0.2f, 1f);
        [Content, Ignore] private float speed = 0f;
        [Content, Ignore] private float direction = 0f;
        [Setting, Ignore] private bool isSprinting = false;
        [Content, Ignore, SerializeField] private Vector3 targetDirection;
        [Content, Ignore, SerializeField] private Vector2 input;
        [Content, Ignore, SerializeField] private Quaternion freeRotation;
        [Resources, SerializeField] private Camera mainCamera;
        [Content] private float velocity;

        [Resources, SerializeField] private Animator anim;
        [Resources, Setting, Header("Animation Property Name")] public string Speed_float = "Speed";
        [Resources, Setting] public string Direction_float = "Direction";
        [Resources, Setting] public string isSprinting_bool = "isSprinting";

        // Use this for initialization
        void Start()
        {
            if (!anim)
                anim = GetComponent<Animator>();
            if (!mainCamera)
                mainCamera = Camera.main;
        }

        // Update is called once per frame
        void FixedUpdate()
        {
            input.x = Input.GetAxis("Horizontal");
            input.y = Input.GetAxis("Vertical");

            // set speed to both vertical and horizontal inputs
            if (useCharacterForward)
                speed = Mathf.Abs(input.x) + input.y;
            else
                speed = Mathf.Abs(input.x) + Mathf.Abs(input.y);

            speed = Mathf.Clamp(speed, 0f, 1f);
            speed = Mathf.SmoothDamp(anim.GetFloat(Speed_float), speed, ref velocity, 0.1f);
            anim.SetFloat(Speed_float, speed);

            if (input.y < 0f && useCharacterForward)
                direction = input.y;
            else
                direction = 0f;

            anim.SetFloat(Direction_float, direction);

            // set sprinting
            isSprinting = ((Input.GetKey(sprintJoystick) || Input.GetKey(sprintKeyboard)) && input != Vector2.zero && direction >= 0f);
            anim.SetBool(isSprinting_bool, isSprinting);

            // Update target direction relative to the camera view (or not if the Keep Direction option is checked)
            UpdateTargetDirection();
            if (input != Vector2.zero && targetDirection.magnitude > 0.1f)
            {
                Vector3 lookDirection = targetDirection.normalized;
                freeRotation = Quaternion.LookRotation(lookDirection, transform.up);
                var diferenceRotation = freeRotation.eulerAngles.y - transform.eulerAngles.y;
                var eulerY = transform.eulerAngles.y;

                if (diferenceRotation < 0 || diferenceRotation > 0) eulerY = freeRotation.eulerAngles.y;
                var euler = new Vector3(0, eulerY, 0);

                transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.Euler(euler), turnSpeed * turnSpeedMultiplier * Time.deltaTime);
            }
        }

        public virtual void UpdateTargetDirection()
        {
            if (!useCharacterForward)
            {
                turnSpeedMultiplier = turnSpeedMultiplierBoundary.x;
                var forward = mainCamera.transform.TransformDirection(Vector3.forward);
                forward.y = 0;

                //get the right-facing direction of the referenceTransform
                var right = mainCamera.transform.TransformDirection(Vector3.right);

                // determine the direction the player will face based on input and the referenceTransform's right and forward directions
                targetDirection = input.x * right + input.y * forward;
            }
            else
            {
                turnSpeedMultiplier = turnSpeedMultiplierBoundary.y;
                var forward = transform.TransformDirection(Vector3.forward);
                forward.y = 0;

                //get the right-facing direction of the referenceTransform
                var right = transform.TransformDirection(Vector3.right);
                targetDirection = input.x * right + Mathf.Abs(input.y) * forward;
            }
        }
    }
}
