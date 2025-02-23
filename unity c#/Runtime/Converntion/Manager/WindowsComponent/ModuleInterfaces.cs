using UnityEngine.Events;

namespace Convention.WindowsUI
{
    public interface IWindowUIModule : IAnyClass { }

    public class WindowUIModule : MonoAnyBehaviour, IWindowUIModule { }

    public interface IText
    {
        string text { get; set; }
    }

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

    public interface IActionInvoke<Args>
    {
        IActionInvoke<Args> AddListener(params UnityAction<Args>[] action);
        IActionInvoke<Args> RemoveListener(params UnityAction<Args>[] action);
        IActionInvoke<Args> RemoveAllListeners();
    }

    public interface IButton : IWindowUIModule, IActionInvoke { }

    public interface IToggle : IButton
    {
        bool toggleValue { get; set; }
    }
}
