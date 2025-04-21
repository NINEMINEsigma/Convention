using UnityEngine;
using UnityEngine.UI;

namespace Convention.WindowsUI.Variant
{
    public class ConsoleListItem : WindowUIModule,IText,ITitle
    {
        [Resources, SerializeField, OnlyNotNullMode] private Text MyTitleText;
        [Resources, SerializeField, OnlyNotNullMode] private Button RawButton;

        public string stackTrace;
        public bool IsEnableFocusWindow;
        public LogType logType;

        public string text { get => ((IText)this.MyTitleText).text; set => ((IText)this.MyTitleText).text = value; }
        public string title { get => ((ITitle)this.MyTitleText).title; set => ((ITitle)this.MyTitleText).title = value; }

        public void SetupMessage(string message, string stackTrace, string color, LogType logType, string format = "<color={color}>{message}</color>")
        {
            format = format.Replace("color", color);
            format = format.Replace("message", message);
            this.title = format;
            this.stackTrace = stackTrace;
            this.logType = logType;
        }

        protected void Start()
        {
            RawButton.onClick.AddListener(OnFocusConsoleItem);
        }

        [Content]
        public void OnFocusConsoleItem()
        {
            ConsoleWindow.instance.SetStackTrace(this.stackTrace);
            if (!IsEnableFocusWindow)
                return;
            if (FocusWindowIndictaor.instance != null)
                FocusWindowIndictaor.instance.SetTargetRectTransform(MyTitleText.transform as RectTransform);
        }
    }
}
