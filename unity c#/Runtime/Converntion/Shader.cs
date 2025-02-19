using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Convention
{
    public static class ShaderExtension
    {
        [ArgPackage,Serializable]
        public class GlobalShaderPropertyWrapper:AnyClass
        {
            public GlobalShaderPropertyWrapper(string name) { this.name = name; }
            [Setting] public string name;
            public float floatValue
            {
                get => Shader.GetGlobalFloat(name);
                set => Shader.SetGlobalFloat(name, value);
            }
            public int intValue
            {
                get => Shader.GetGlobalInt(name);
                set => Shader.SetGlobalInt(name, value);
            }
            public Color colorValue
            {
                get => Shader.GetGlobalColor(name);
                set => Shader.SetGlobalColor(name, value);
            }
            public Texture textureValue
            {
                get => Shader.GetGlobalTexture(name);
                set => Shader.SetGlobalTexture(name, value);
            }
        }
        public static GlobalShaderPropertyWrapper WrapperName2GlobalFloatProperty(string name) => new GlobalShaderPropertyWrapper(name);
        public static int PropertyToID([In] string name) => Shader.PropertyToID(name);
        public static int ToShaderPropertyID([In] this string self) => PropertyToID(self);
    }
}
