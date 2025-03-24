using System;
using System.Collections;
using System.Collections.Generic;
using Convention.WindowsUI;
using Convention.WindowsUI.Variant;
using UnityEngine;
using UnityEngine.Events;

namespace Convention.Workflow
{
    public class GraphInputWindow : MonoSingleton<GraphInputWindow>, ITitle, IText
    {
        [Resources, SerializeField, OnlyNotNullMode] private Text Title;
        [Resources, SerializeField, OnlyNotNullMode] private Text Description;
        [Resources, SerializeField, OnlyNotNullMode] private ModernUIButton AddContentInput;

        public string title { get => ((ITitle)this.Title).title; set => ((ITitle)this.Title).title = value; }
        public string text { get => ((IText)this.Description).text; set => ((IText)this.Description).text = value; }

        private void Start()
        {
            AddContentInput.AddListener(() =>
            {
                SharedModule.instance.OpenCustomMenu(this.transform as RectTransform, new SharedModule.CallbackData("test", go => { }));
            });
        }

    }
}
