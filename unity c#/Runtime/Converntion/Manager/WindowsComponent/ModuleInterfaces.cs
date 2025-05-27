using System.Text.RegularExpressions;
using UnityEngine;
using UnityEngine.Events;

namespace Convention.WindowsUI
{
    public interface IWindowUIModule : IAnyClass { }

    public class WindowUIModule : MonoAnyBehaviour, IWindowUIModule { }

    public static partial class UIInterfaceExtension
    {
        public static void AutoTransform(GameObject @object)
        {
            if (@object == null) return;
            foreach (var text in @object.GetComponentsInChildren<IText>(true))
            {
                text.Transform();
            }
            foreach (var title in @object.GetComponentsInChildren<ITitle>(true))
            {
                title.Transform();
            }
            foreach (Transform child in @object.transform)
            {
                AutoTransform(child.gameObject);
            }
        }

        public static void Transform(this IText self)
        {
            try
            {
                string result = self.text;
                foreach (Match match in Regex.Matches(self.text, @"\$\$.*?\$\$"))
                {
                    result = result.Replace(match.Value, StringExtension.Transform(match.Value[2..^2]));
                }
                self.text = result;
            }
            catch (System.Exception) { }
        }

        public static void Transform(this ITitle self)
        {
            try
            {
                string result = self.title;
                foreach (Match match in Regex.Matches(self.title, @"\$\$.*?\$\$"))
                {
                    result = result.Replace(match.Value, StringExtension.Transform(match.Value[2..^2]));
                }
                self.title = result;
            }
            catch (System.Exception) { }
        }
    }

    public interface IText : IAnyClass
    {
        string text { get; set; }
    }

    public interface ITitle : IAnyClass
    {
        string title { get; set; }
    }

    public interface IInteractable : IAnyClass
    {
        bool interactable { get; set; }
    }

    public interface IActionInvoke : IInteractable
    {
        IActionInvoke AddListener(params UnityAction[] action);
        IActionInvoke RemoveListener(params UnityAction[] action);
        IActionInvoke RemoveAllListeners();
    }

    public interface IActionInvoke<Args> : IInteractable
    {
        IActionInvoke<Args> AddListener(params UnityAction<Args>[] action);
        IActionInvoke<Args> RemoveListener(params UnityAction<Args>[] action);
        IActionInvoke<Args> RemoveAllListeners();
    }

    public interface IButton : IWindowUIModule, IActionInvoke { }

    public interface IToggle : IWindowUIModule, IActionInvoke<bool>, IGetRefValue<bool> { }

    public interface IInputField : IWindowUIModule, IActionInvoke<string>, IText { }
}
