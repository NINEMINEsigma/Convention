using UnityEngine.Events;

namespace Convention.WindowsUI
{
    public partial class ModernUIInputField : WindowUIModule, ITitle, IInputField
    {
        [Resources, HopeNotNull] public Text TitleSource;
        [Resources, HopeNotNull] public InputField InputFieldSource;

        public virtual string text { get => InputFieldSource.text; set => InputFieldSource.text = value; }
        public virtual string title { get => TitleSource.title; set => TitleSource.title = value; }
        public bool interactable { get => InputFieldSource.Source.interactable; set => InputFieldSource.Source.interactable = value; }

        public IActionInvoke<string> AddListener(params UnityAction<string>[] action)
        {
            return ((IActionInvoke<string>)this.InputFieldSource).AddListener(action);
        }

        public IActionInvoke<string> RemoveAllListeners()
        {
            return ((IActionInvoke<string>)this.InputFieldSource).RemoveAllListeners();
        }

        public IActionInvoke<string> RemoveListener(params UnityAction<string>[] action)
        {
            return ((IActionInvoke<string>)this.InputFieldSource).RemoveListener(action);
        }

        private void Reset()
        {
            TitleSource = GetComponentInChildren<Text>();
            InputFieldSource = GetComponentInChildren<InputField>();
        }


    }
}