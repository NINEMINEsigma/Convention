using Convention.Benchmarking;
using UnityEngine;
using UnityEngine.InputSystem;

namespace Convention
{
    public class PlayerInput : MonoAnyBehaviour
    {
        [Header("Need \"Horizontal\" and \"Vertical\" for movement, \"Sprint\" and \"Jump\" for behaviour")]
        [Setting, HopeNotNull] public InputActionAsset inputAction;
        [Setting] public bool useCharacterForward = false;
        [Setting] public bool lockToCameraForward = false;
        [Setting] public float turnSpeed = 10f;
        [Setting] public Vector2 turnSpeedMultiplierBoundary = new(0.2f, 0.4f);

        [Content, Ignore] private float turnSpeedMultiplier;
        [Content, Ignore] private float speed = 0f;
        [Content, Ignore] private float lastSpeed = 0f;
        [Content, Ignore] private float direction = 0f;
        [Header("Input Infomation")]
        [Setting, OnlyNotNullMode(nameof(inputAction))] public int InputSelectIndex = 0;
        [Content, Ignore, SerializeField] private Vector2 input;
        [Content, Ignore] private bool isInputSprinting = false;
        [Content, Ignore] private bool IsInputJumping = false;
        [Content, Ignore, SerializeField] private Vector3 targetDirection;
        [Content, Ignore, SerializeField] private Quaternion freeRotation;
        [Resources, SerializeField, HopeNotNull] private Camera mainCamera;
        [Content, Ignore, SerializeField] private float velocity;

        [Resources, SerializeField, HopeNotNull] private Animator anim;
        [Resources, Setting, Header("Animation Property Name"), OnlyNotNullMode(nameof(anim))] public string Speed_float = "Speed";
        [Resources, Setting, OnlyNotNullMode(nameof(anim))] public string Direction_float = "Direction";
        [Resources, Setting, OnlyNotNullMode(nameof(anim))] public string isSprinting_bool = "isSprinting";

        void Start()
        {
            if (!anim)
                anim = GetComponent<Animator>();
            if (!mainCamera)
                mainCamera = Camera.main;
            if (!inputAction)
                inputAction = Resources.Load<InputActionAsset>("FPS");
        }

        protected virtual void RefreshInput()
        {
            input.x = inputAction.actionMaps[InputSelectIndex]["Horizontal"].ReadValue<float>();
            input.y = inputAction.actionMaps[InputSelectIndex]["Vertical"].ReadValue<float>();
            isInputSprinting = !Mathf.Approximately(inputAction.actionMaps[InputSelectIndex]["Sprint"].ReadValue<float>(), 0) &&
                input != Vector2.zero &&
                direction >= 0f;
            IsInputJumping = !Mathf.Approximately(inputAction.actionMaps[InputSelectIndex]["Jump"].ReadValue<float>(), 0);
        }

        void FixedUpdate()
        {
            RefreshInput();
            // set speed to both vertical and horizontal inputs
            if (useCharacterForward)
                speed = Mathf.Abs(input.x) + input.y;
            else
                speed = Mathf.Abs(input.x) + Mathf.Abs(input.y);

            speed = Mathf.Clamp(speed, 0f, 1f);
            if (anim == null || anim.applyRootMotion == false)
            {
                lastSpeed = speed = Mathf.SmoothDamp(lastSpeed, speed, ref velocity, 0.1f);
                Movement();
            }
            else if (anim)
            {
                lastSpeed = speed = Mathf.SmoothDamp(anim.GetFloat(Speed_float), speed, ref velocity, 0.1f);
                anim.SetFloat(Speed_float, speed);
                if (anim.applyRootMotion == false)
                {
                    Movement();
                }
            }
            void Movement()
            {
                if (Mathf.Approximately(speed, 0) == false)
                {
                    if (useCharacterForward)
                        transform.Translate(new(Time.deltaTime * input.x, 0, Time.deltaTime * input.y), Space.Self);
                    else
                    {
                        var move =
                           Time.deltaTime * input.x * mainCamera.transform.right +
                           Time.deltaTime * input.y * mainCamera.transform.forward;
                        move.y = 0;
                        transform.Translate(move, Space.Self);
                    }
                }
            }

            if (input.y < 0f && useCharacterForward)
                direction = input.y;
            else
                direction = 0f;

            if (anim)
            {
                anim.SetFloat(Direction_float, direction);
            }

            // set sprinting
            if (anim)
            {
                anim.SetBool(isSprinting_bool, isInputSprinting);
            }

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

        private void OnApplicationFocus(bool focus)
        {
            SetCursorState(focus);
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

        private void SetCursorState(bool newState)
        {
            if (PerformanceTestManager.RunningBenchmark)
                return;

            //Cursor.lockState = newState ? CursorLockMode.Locked : CursorLockMode.None;
        }
    }
}

