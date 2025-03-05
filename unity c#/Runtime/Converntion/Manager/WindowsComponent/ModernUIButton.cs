using System;
using System.Collections;
using TMPro;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using Convention.WindowsUI.Internal;
#if ENABLE_INPUT_SYSTEM
using UnityEngine.InputSystem;
#endif

namespace Convention.WindowsUI
{
    namespace Internal
    {
        public class Ripple : MonoBehaviour
        {
            public bool unscaledTime = false;
            public float speed;
            public float maxSize;
            public Color startColor;
            public Color transitionColor;
            UnityEngine.UI.Image colorImg;

            void Start()
            {
                transform.localScale = new Vector3(0f, 0f, 0f);
                colorImg = GetComponent<UnityEngine.UI.Image>();
                colorImg.color = new Color(startColor.r, startColor.g, startColor.b, startColor.a);
            }

            void Update()
            {
                if (unscaledTime == false)
                {
                    transform.localScale = Vector3.Lerp(transform.localScale, new Vector3(maxSize, maxSize, maxSize), Time.deltaTime * speed);
                    colorImg.color = Color.Lerp(colorImg.color, new Color(transitionColor.r, transitionColor.g, transitionColor.b, transitionColor.a), Time.deltaTime * speed);

                    if (transform.localScale.x >= maxSize * 0.998)
                    {
                        if (transform.parent.childCount == 1)
                            transform.parent.gameObject.SetActive(false);

                        Destroy(gameObject);
                    }
                }

                else
                {
                    transform.localScale = Vector3.Lerp(transform.localScale, new Vector3(maxSize, maxSize, maxSize), Time.unscaledDeltaTime * speed);
                    colorImg.color = Color.Lerp(colorImg.color, new Color(transitionColor.r, transitionColor.g, transitionColor.b, transitionColor.a), Time.unscaledDeltaTime * speed);

                    if (transform.localScale.x >= maxSize * 0.998)
                    {
                        if (transform.parent.childCount == 1)
                            transform.parent.gameObject.SetActive(false);

                        Destroy(gameObject);
                    }
                }
            }
        }
    }

    public partial class ModernUIButton : WindowUIModule, IButton, ITitle
    {
        // ---------------------------------------------------------------- stats

        private bool useRipple => animationSolution == AnimationSolution.SCRIPT && m_useRipple;

        // ----------------------------------------------------------------

        public enum RippleUpdateMode
        {
            NORMAL,
            UNSCALED_TIME
        }
        public enum AnimationSolution
        {
            ANIMATOR,
            SCRIPT
        }
        [Content, SerializeField] private string m_title = "";
        public string title
        {
            get => m_title;
            set
            {
                m_title = value;
                UpdateUI();
            }
        }
        [Content] public UnityEvent clickEvent = new();
        [Content] public UnityEvent hoverEvent = new();

        [Resources, OnlyNotNullMode] public TextMeshProUGUI normalText;
        [Resources, HopeNotNull] public CanvasGroup normalCanvasGroup;
        [Resources, OnlyNotNullMode] public TextMeshProUGUI highlightedText;
        [Resources, HopeNotNull] public CanvasGroup highlightedCanvasGroup;

        [Resources, Setting, HopeNotNull, Header("Sound Setting")] public AudioSource soundSource;
        [Resources, Setting, OnlyNotNullMode(nameof(soundSource))] public bool enableButtonSounds = false;
        [Resources, Setting, OnlyNotNullMode(nameof(soundSource))] public bool useHoverSound = true;
        [Resources, Setting, OnlyNotNullMode(nameof(soundSource))] public bool useClickSound = true;
        [Resources, WhenAttribute.Is(nameof(useHoverSound), true), OnlyNotNullMode(nameof(soundSource))] public AudioClip hoverSound;
        [Resources, WhenAttribute.Is(nameof(useClickSound), true), OnlyNotNullMode(nameof(soundSource))] public AudioClip clickSound;

        [Resources, Setting, SerializeField, Header("Animation Setting")]
        private AnimationSolution animationSolution = AnimationSolution.SCRIPT;
        [Resources, WhenAttribute.Is(nameof(animationSolution), AnimationSolution.SCRIPT), OnlyNotNullMode]
        public GameObject rippleParent;
        [Resources, WhenAttribute.Is(nameof(animationSolution), AnimationSolution.SCRIPT), Setting, SerializeField]
        private bool m_useRipple = true;
        [WhenAttribute.Is(nameof(useRipple), true), Setting] public bool renderOnTop = false;
        [WhenAttribute.Is(nameof(useRipple), true), Setting] public bool centered = false;
        [WhenAttribute.Is(nameof(useRipple), true), Setting] public RippleUpdateMode rippleUpdateMode = RippleUpdateMode.UNSCALED_TIME;
        [WhenAttribute.Is(nameof(useRipple), true), Setting, Range(0.25f, 15)] public float fadingMultiplier = 8;
        [WhenAttribute.Is(nameof(useRipple), true), Setting, Range(0.1f, 5)] public float speed = 1f;
        [WhenAttribute.Is(nameof(useRipple), true), Setting, Range(0.5f, 25)] public float maxSize = 4f;
        [WhenAttribute.Is(nameof(useRipple), true), Setting] public Color startColor = new Color(1f, 1f, 1f, 1f);
        [WhenAttribute.Is(nameof(useRipple), true), Setting] public Color transitionColor = new Color(1f, 1f, 1f, 1f);
        [WhenAttribute.Is(nameof(useRipple), true), Resources, HopeNotNull] public Sprite rippleShape;
        [WhenAttribute.Is(nameof(useRipple), true), Setting] public bool hoverCreateRipple = false;
        [WhenAttribute.Is(nameof(useRipple), true), Setting] public bool exitCreateRipple = false;

        //[Header("Others")]
        [Content, Ignore] private bool isPointerNotExit;
        [Content, Ignore] private float currentNormalValue;
        [Content, Ignore] private float currenthighlightedValue;
        [Setting, SerializeField] private bool m_interactable = true;
        public bool interactable
        {
            get => m_interactable;
            set => m_interactable = value;
        }

        private void Start()
        {
            ResetContext();

            if (animationSolution == AnimationSolution.SCRIPT)
            {
                if (normalCanvasGroup == null)
                    normalCanvasGroup = transform.Find("Normal").GetComponent<CanvasGroup>();
                if (highlightedCanvasGroup == null)
                    highlightedCanvasGroup = transform.Find("Highlighted").GetComponent<CanvasGroup>();

                Animator tempAnimator = this.GetComponent<Animator>();
                Destroy(tempAnimator);
            }

            if (rippleParent)
            {
                if (useRipple)
                    rippleParent.SetActive(false);
                else
                    Destroy(rippleParent);
            }
        }

        private void OnValidate()
        {
            UpdateUI();
        }

        public void ResetContext()
        {
            var Context = this.GetOrAddComponent<BehaviourContextManager>();
            Context.OnPointerDownEvent = BehaviourContextManager.InitializeContextSingleEvent(Context.OnPointerClickEvent, OnPointerDown);
            Context.OnPointerEnterEvent = BehaviourContextManager.InitializeContextSingleEvent(Context.OnPointerEnterEvent, OnPointerEnter);
            Context.OnPointerExitEvent = BehaviourContextManager.InitializeContextSingleEvent(Context.OnPointerExitEvent, OnPointerExit);

        }
        public void Reset()
        {
            ResetContext();
            if (normalCanvasGroup == null)
                normalCanvasGroup = transform.Find("Normal").GetComponent<CanvasGroup>();
            if (highlightedCanvasGroup == null)
                highlightedCanvasGroup = transform.Find("Highlighted").GetComponent<CanvasGroup>();
        }

        void OnEnable()
        {
            UpdateUI();
            normalCanvasGroup.alpha = 1;
            highlightedCanvasGroup.alpha = 0;
        }

        [Setting, Header("Is Auto Rename"), SerializeField] private bool m_IsAutoRename = true;
        public void UpdateUI()
        {
            normalText.text = m_title;
            highlightedText.text = m_title;
            if (m_IsAutoRename)
            {
                this.name = $"{this.GetType().Name}<{m_title}>";
            }
        }

        public void CreateRipple(Vector2 pos)
        {
            if (rippleParent != null)
            {
                GameObject rippleObj = new GameObject();
                rippleObj.AddComponent<UnityEngine.UI.Image>();
                rippleObj.GetComponent<UnityEngine.UI.Image>().sprite = rippleShape;
                rippleObj.name = "Ripple";
                rippleParent.SetActive(true);
                rippleObj.transform.SetParent(rippleParent.transform);

                if (renderOnTop == true)
                    rippleParent.transform.SetAsLastSibling();
                else
                    rippleParent.transform.SetAsFirstSibling();

                if (centered == true)
                    rippleObj.transform.localPosition = new Vector2(0f, 0f);
                else
                    rippleObj.transform.position = pos;

                rippleObj.AddComponent<Ripple>();
                Ripple tempRipple = rippleObj.GetComponent<Ripple>();
                tempRipple.speed = speed;
                tempRipple.maxSize = maxSize;
                tempRipple.startColor = startColor;
                tempRipple.transitionColor = transitionColor;

                if (rippleUpdateMode == RippleUpdateMode.NORMAL)
                    tempRipple.unscaledTime = false;
                else
                    tempRipple.unscaledTime = true;
            }
        }

        public void OnPointerCreateRipple(PointerEventData eventData)
        {
            if (useRipple == true && isPointerNotExit == true)
#if ENABLE_LEGACY_INPUT_MANAGER
                CreateRipple(Input.mousePosition);
#elif ENABLE_INPUT_SYSTEM && ENABLE_LEGACY_INPUT_MANAGER
                CreateRipple(Input.mousePosition);
#elif ENABLE_INPUT_SYSTEM
                CreateRipple(Mouse.current.position.ReadValue());
#endif
            else if (useRipple == false)
                this.enabled = false;
        }

        public void OnPointerDown(PointerEventData eventData)
        {
            if (interactable == false)
                return;
            if (enableButtonSounds == true && useClickSound == true)
                soundSource.PlayOneShot(clickSound);

            clickEvent.Invoke();

            OnPointerCreateRipple(eventData);
        }

        private void OnPointerEnter(PointerEventData eventData)
        {
            if (interactable == false)
                return;
            if (enableButtonSounds == true && useHoverSound == true)
                soundSource.PlayOneShot(hoverSound);

            hoverEvent.Invoke();
            isPointerNotExit = true;

            if (animationSolution == AnimationSolution.SCRIPT)
                StartCoroutine(nameof(FadeIn));

            if (hoverCreateRipple)
                OnPointerCreateRipple(eventData);
        }

        private void OnPointerExit(PointerEventData eventData)
        {
            if (interactable == false)
                return;
            if (exitCreateRipple)
                OnPointerCreateRipple(eventData);

            isPointerNotExit = false;

            if (animationSolution == AnimationSolution.SCRIPT)
                StartCoroutine(nameof(FadeOut));
        }

        IEnumerator FadeIn()
        {
            StopCoroutine("FadeOut");
            currentNormalValue = normalCanvasGroup.alpha;
            currenthighlightedValue = highlightedCanvasGroup.alpha;

            while (currenthighlightedValue <= 1)
            {
                currentNormalValue -= Time.deltaTime * fadingMultiplier;
                normalCanvasGroup.alpha = currentNormalValue;

                currenthighlightedValue += Time.deltaTime * fadingMultiplier;
                highlightedCanvasGroup.alpha = currenthighlightedValue;

                if (normalCanvasGroup.alpha >= 1)
                    StopCoroutine("FadeIn");

                yield return null;
            }
        }

        IEnumerator FadeOut()
        {
            StopCoroutine("FadeIn");
            currentNormalValue = normalCanvasGroup.alpha;
            currenthighlightedValue = highlightedCanvasGroup.alpha;

            while (currentNormalValue >= 0)
            {
                currentNormalValue += Time.deltaTime * fadingMultiplier;
                normalCanvasGroup.alpha = currentNormalValue;

                currenthighlightedValue -= Time.deltaTime * fadingMultiplier;
                highlightedCanvasGroup.alpha = currenthighlightedValue;

                if (highlightedCanvasGroup.alpha <= 0)
                    StopCoroutine("FadeOut");

                yield return null;
            }
        }

        public IActionInvoke AddListener(params UnityAction[] action)
        {
            foreach (var actionItem in action)
                clickEvent.AddListener(actionItem);
            return this;
        }

        public IActionInvoke RemoveListener(params UnityAction[] action)
        {
            foreach (var actionItem in action)
                clickEvent.RemoveListener(actionItem);
            return this;
        }

        public IActionInvoke RemoveAllListeners()
        {
            clickEvent.RemoveAllListeners();
            return this;
        }

        public Color NormalColor
        {
            get => normalCanvasGroup.GetComponentInChildren<Image>().color;
            set
            {
                normalCanvasGroup.GetComponentInChildren<Image>().color = value;
                normalCanvasGroup.GetComponentInChildren<TMP_Text>().color = value;
            }
        }
    }
}
