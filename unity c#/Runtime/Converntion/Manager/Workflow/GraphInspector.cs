using System;
using System.Collections;
using System.Collections.Generic;
using Convention.WindowsUI;
using Convention.WindowsUI.Variant;
using UnityEngine;
using UnityEngine.Events;

namespace Convention.Workflow
{
    public class GraphInspector : MonoSingleton<GraphInspector>, ITitle, IText
    {
        [Resources, SerializeField, OnlyNotNullMode] private Text Title;

        private RegisterWrapper<GraphInspector> RegisterWrapper;

        public string title { get => ((ITitle)this.Title).title; set => ((ITitle)this.Title).title = value; }
        public string text { get => ((IText)this.Title).text; set => ((IText)this.Title).text = value; }

        private void Start()
        {
            RegisterWrapper = new(() => { });
        }
    }
}
