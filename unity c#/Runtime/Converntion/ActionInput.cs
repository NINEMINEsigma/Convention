using System.Collections.Generic;
using Sirenix.OdinInspector;
using UnityEngine.Events;
using UnityEngine.InputSystem;

namespace Convention
{
    public class ActionInput : SerializedMonoBehaviour, IAnyClass
    {
        [Setting] public Dictionary<InputAction, UnityEvent> InputEvent = new();
        [Content] public RegisterWrapper<ActionInput> m_RegisterWrapper;

        [return: ReturnNotNull]
        public string SymbolName()
        {
            return nameof(InputAction);
        }

        private void Start()
        {
            m_RegisterWrapper = new RegisterWrapper<ActionInput>(() => { });
            foreach (var item in InputEvent)
            {
                item.Key.performed += (call) => item.Value.Invoke();
                item.Key.Enable();
            }
        }

        private void OnDestroy()
        {
            m_RegisterWrapper.Release();
        }
    }
}
