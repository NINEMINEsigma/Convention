using UnityEngine.Events;

namespace Convention.WindowsUI
{
    public interface IWindowUIModule { }

    public interface ITitle
    {
        string title { get; set; }
    }

    public interface IActionInvoke
    {
        IActionInvoke AddListener(params UnityAction[] action);
        IActionInvoke RemoveListener(params UnityAction[] action);
        IActionInvoke RemoveAllListeners();
    }

    public interface IButton : IWindowUIModule, IActionInvoke { }

    public interface IToggle : IButton, ISetRefValue<bool>, IGetRefalue<bool> { }
}
