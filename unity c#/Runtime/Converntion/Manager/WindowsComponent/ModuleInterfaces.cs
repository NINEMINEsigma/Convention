using UnityEngine.Events;

namespace Convention.WindowsUI
{
    public interface IWindowUIModule : IAnyClass { }

    public class WindowUIModule : MonoAnyBehaviour, IWindowUIModule { }

    public interface IText:IAnyClass
    {
        string text { get; set; }
    }

    public interface ITitle:IAnyClass
    {
        string title { get; set; }
    }

    public interface IInteractable:IAnyClass
    {
        bool interactable { get; set;}
    }

    public interface IActionInvoke: IInteractable
    {
        IActionInvoke AddListener(params UnityAction[] action);
        IActionInvoke RemoveListener(params UnityAction[] action);
        IActionInvoke RemoveAllListeners();
    }

    public interface IActionInvoke<Args>: IInteractable
    {
        IActionInvoke<Args> AddListener(params UnityAction<Args>[] action);
        IActionInvoke<Args> RemoveListener(params UnityAction<Args>[] action);
        IActionInvoke<Args> RemoveAllListeners();
    }

    public interface IButton : IWindowUIModule, IActionInvoke { }

    public interface IToggle : IWindowUIModule, IActionInvoke<bool>, IGetRefValue<bool> { }

    public interface IInputField : IWindowUIModule, IActionInvoke<string>, IText { }
}
