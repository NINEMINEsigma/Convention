using System;
using TMPro;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

namespace Convention.WindowsUI
{
    [Serializable]
    public class ModernUIToggle : WindowUIModule, IToggle, ITitle
    {
        [Content] public UnityEvent m_SwitchOnEvent = new();
        [Content] public UnityEvent m_SwitchOffEvent = new();
        [Content] public UnityEvent<bool> m_ToggleEvent = new();

        [Resources, OnlyNotNullMode] public TextMeshProUGUI normalText;
        [Resources, HopeNotNull] public CanvasGroup normalCanvasGroup;
        [Resources, OnlyNotNullMode] public TextMeshProUGUI selectedText;
        [Resources, HopeNotNull] public CanvasGroup selectedCanvasGroup;

        [Resources, Setting, HopeNotNull, Header("Sound Setting")] public AudioSource soundSource;
        [Resources, Setting, OnlyNotNullMode(nameof(soundSource))] public bool enableButtonSounds = false;
        [Resources, WhenAttribute.Is(nameof(enableButtonSounds), true), OnlyNotNullMode(nameof(soundSource))] public AudioClip m_SwitchOnSound;
        [Resources, WhenAttribute.Is(nameof(enableButtonSounds), true), OnlyNotNullMode(nameof(soundSource))] public AudioClip m_SwitchOffSound;
        [Content, SerializeField] private string m_title = "";
        [Content, SerializeField] private bool m_value = false;

        public bool ref_value
        {
            get => m_value;
            set
            {
                if (m_value != value)
                {
                    m_value = value;
                    normalCanvasGroup.alpha = m_value ? 0 : 1;
                    selectedCanvasGroup.alpha = m_value ? 1 : 0;
                }
            }
        }

        public string title
        {
            get => m_title;
            set
            {
                m_title = value;
                UpdateUI();
            }
        }

        private void Start()
        {
            ResetContext();
        }
        private void OnValidate()
        {
            UpdateUI();
        }

        public void Reset()
        {
            ResetContext();
            if (normalCanvasGroup == null)
                normalCanvasGroup = transform.Find("Normal").GetComponent<CanvasGroup>();
            if (selectedCanvasGroup == null)
                selectedCanvasGroup = transform.Find("Selected").GetComponent<CanvasGroup>();
        }
        void OnEnable()
        {
            UpdateUI();
        }
        public void ResetContext()
        {
            var Context = this.GetOrAddComponent<BehaviourContextManager>();
            Context.OnPointerDownEvent = BehaviourContextManager.InitializeContextSingleEvent(Context.OnPointerClickEvent, OnPointerDown);

        }
        public void UpdateUI()
        {
            normalText.text = m_title;
            selectedText.text = m_title;
            normalCanvasGroup.alpha = m_value ? 0 : 1;
            selectedCanvasGroup.alpha = m_value ? 1 : 0;
        }

        public void OnPointerDown(PointerEventData eventData)
        {
            ref_value = !ref_value;
            if (enableButtonSounds == true)
                soundSource.PlayOneShot(ref_value ? m_SwitchOnSound : m_SwitchOffSound);
            (ref_value ? m_SwitchOnEvent : m_SwitchOffEvent).Invoke();
        }

        public IActionInvoke<bool> AddListener(params UnityAction<bool>[] action)
        {
            foreach (var item in action)
            {
                m_ToggleEvent.AddListener(item);
            }
            return this;
        }

        public IActionInvoke<bool> RemoveListener(params UnityAction<bool>[] action)
        {
            foreach (var item in action)
            {
                m_ToggleEvent.RemoveListener(item);
            }
            return this;
        }

        public IActionInvoke<bool> RemoveAllListeners()
        {
            m_ToggleEvent.RemoveAllListeners();
            return this;
        }
    }
}
