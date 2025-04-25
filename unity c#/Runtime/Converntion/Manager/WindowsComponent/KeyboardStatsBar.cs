using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

namespace Convention.WindowsUI
{
    public class KeyboardStatsBar : WindowUIModule
    {
        [Serializable]
        public class KeyboardStatsData
        {
            public Key key;
            public CanvasGroup iconCanvasGroup;
            public float notPress = 0.3f;
            public float press = 1f;
        }

        [Setting] public List<KeyboardStatsData> bindings = new();

        private void Update()
        {
            foreach (var bind in bindings)
            {
                bind.iconCanvasGroup.alpha = Keyboard.current[bind.key].isPressed ? bind.press : bind.notPress;
            }
        }
    }
}
