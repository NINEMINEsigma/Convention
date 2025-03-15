using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Convention.WindowsUI.Variant
{
    public class SharedModule : MonoSingleton<SharedModule>, IWindowUIModule
    {
        [Resources, OnlyNotNullMode, SerializeField] private ModernUIInputField RenameField;
        private Action<string> RenameCallback;

        private void Start()
        {
            RenameField.AddListener(x =>
            {
                RenameCallback(x);
                RenameField.gameObject.SetActive(false);
            });
        }

        public void Rename(string initText, Action<string> callback)
        {
            RenameField.gameObject.SetActive(true);
            RenameField.text = initText;
            RenameCallback = callback;
        }
    }
}
