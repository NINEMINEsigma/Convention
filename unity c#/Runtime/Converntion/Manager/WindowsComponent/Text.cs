using TMPro;
using UnityEngine;

namespace Convention.WindowsUI
{
    public partial class Text : WindowUIModule, IText, ITitle
    {
        [Resources, HopeNotNull] public TextMeshProUGUI source;
        public virtual string text
        {
            get => source.text;
            set => source.text = value;
        }
        public virtual string title
        {
            get => source.text;
            set => source.text = value;
        }

        private void Start()
        {
            if (source == null)
                source = this.GetComponent<TextMeshProUGUI>();
        }

        private void Reset()
        {
            if (source == null)
                source = this.GetComponent<TextMeshProUGUI>();
        }
    }
}
