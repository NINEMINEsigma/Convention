using System;
using System.Collections.Generic;
using System.Linq;

namespace Convention
{
    public static class RegisterBaseWrapperExtension
    {
        public static void InitExtensionEnv()
        {
            Registers = new();
        }
        internal static Dictionary<Type, RegisterBaseWrapper> Registers = new();
    }
    public abstract class RegisterBaseWrapper : AnyClass
    {
        internal abstract void OnceRegister(Type current);
    }
    /// <summary>
    /// <b>Constructor</b>
    /// <list type="bullet">set up fields</list>
    /// <list type="bullet">set up <see cref="map_dependences"/></list>
    /// <list type="bullet"><see cref="map_dependences"/> remove exist dependences</list>
    /// If <see cref="map_dependences"/> is empty
    /// <list type="bullet"><see cref="callback"/> will be invoke</list>
    /// <list type="bullet"><see cref="OnceRegister"/> will be invoke</list>
    /// <b><see cref="OnceRegister"/></b>
    /// <list type="bullet">if <see cref="map_dependences"/> not empty, remove type which is currently register successful</list>
    /// <list type="bullet">if then <see cref="map_dependences"/> empty, <see cref="callback"/> and 
    /// <see cref="OnceRegister"/> of every wait-register type will be invoke</list>
    /// <b><see cref="callback"/></b>
    /// <list type="bullet">callback when <see cref="dependences"/> is all register successful</list>
    /// </summary>
    /// <typeparam name="Tag"></typeparam>
    public class RegisterWrapper<Tag> : RegisterBaseWrapper
    {
        private static Action callback;
        public readonly Type[] dependences;
        private HashSet<Type> map_dependences;
        internal override void OnceRegister(Type current)
        {
            if (map_dependences.Count > 0)
            {
                map_dependences.Remove(current);
                if (map_dependences.Count == 0)
                {
                    callback?.Invoke();
                    callback = null;
                    foreach (var item in RegisterBaseWrapperExtension.Registers)
                    {
                        item.Value.OnceRegister(typeof(Tag));
                    }
                }
            }
        }
        public RegisterWrapper(Action callback, params Type[] dependences)
        {
            RegisterWrapper<Tag>.callback += callback;
            this.dependences = dependences;
            this.map_dependences = dependences.ToHashSet();
            this.map_dependences.RemoveWhere((type) => RegisterBaseWrapperExtension.Registers.ContainsKey(type));
            if (this.map_dependences.Count == 0)
            {
                RegisterWrapper<Tag>.callback();
                RegisterWrapper<Tag>.callback = null;
                foreach (var item in RegisterBaseWrapperExtension.Registers)
                {
                    item.Value.OnceRegister(typeof(Tag));
                }
            }
            RegisterBaseWrapperExtension.Registers[typeof(Tag)] = this;
        }
        ~RegisterWrapper()
        {
            Release();
        }
        public bool IsRelease { get; private set; }
        public void Release()
        {
            if (IsRelease == false)
            {
                RegisterBaseWrapperExtension.Registers.Remove(typeof(Tag));
                IsRelease = true;
            }
        }
    }
}
