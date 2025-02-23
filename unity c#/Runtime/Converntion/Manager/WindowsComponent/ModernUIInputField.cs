

namespace Convention.WindowsUI
{
    public partial class ModernUIInputField : WindowUIModule, IText, ITitle
    {
        [Resources, HopeNotNull] public Text TitleSource;
        [Resources, HopeNotNull] public InputField InputFieldSource;

        public virtual string text { get => InputFieldSource.text; set => InputFieldSource.text = value; }
        public virtual string title { get => TitleSource.title; set => TitleSource.title=value; }

        private void Reset()
        {
            TitleSource = GetComponentInChildren<Text>();
            InputFieldSource = GetComponentInChildren<InputField>();
        }
    }
}