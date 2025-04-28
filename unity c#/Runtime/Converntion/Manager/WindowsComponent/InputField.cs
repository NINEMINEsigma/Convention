using TMPro;
using UnityEngine;
using UnityEngine.Events;

namespace Convention.WindowsUI
{
    public partial class InputField : WindowUIModule, IInputField
    {
        [Resources, HopeNotNull] public TMP_InputField Source;
        [Resources] public TMP_Text placeholder;

        private void Start()
        {
            if (Source == null)
                Source = this.GetComponent<TMP_InputField>();
            if (placeholder == null)
                placeholder = Source.placeholder.GetComponent<TMP_Text>();
        }

        private void OnValidate()
        {
            if (Source == null)
                Source = this.GetComponent<TMP_InputField>();
            if (placeholder == null)
                placeholder = Source.placeholder.GetComponent<TMP_Text>();
        }

        public virtual string text
        {
            get { return Source.text; }
            set { Source.text = value; }
        }

        public bool interactable { get => Source.interactable; set => Source.interactable = value; }

        public void SetPlaceholderText(string text)
        {
            placeholder.text = text;
        }

        public InputField SetTextWithoutNotify(string text)
        {
            Source.SetTextWithoutNotify(text);
            return this;
        }

        public IActionInvoke<string> AddListener(params UnityAction<string>[] action)
        {
            foreach (var actionItem in action)
                Source.onEndEdit.AddListener(actionItem);
            return this;
        }

        public IActionInvoke<string> RemoveListener(params UnityAction<string>[] action)
        {
            foreach (var actionItem in action)
                Source.onEndEdit.RemoveListener(actionItem);
            return this;
        }

        public IActionInvoke<string> RemoveAllListeners()
        {
            Source.onEndEdit.RemoveAllListeners();
            return this;
        }
    }
}
