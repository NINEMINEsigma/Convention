using System;
using System.Collections;
using System.Collections.Generic;
using Convention.Internal;
using UnityEngine;

namespace Convention
{
    public interface ITypeClass
    {
        [return: ReturnNotNull] string SymbolName();
    }
    public static class ITypeClassExtension
    {
        public static bool Is<T>(this ITypeClass self) => self.GetType() == typeof(T) || self.GetType().IsSubclassOf(typeof(T));
        [return: ReturnMayNull] public static T As<T>(this ITypeClass self) where T : class => self as T;
        public static T IfIam<T>(this T self, [In][Opt] Action action) where T : ITypeClass
        {
            if (self.Is<T>() && action != null)
                action();
            return self;
        }
        public static T IfIam<T>(this T self, [In][Opt] Action<ITypeClass> action) where T : ITypeClass
        {
            if (self.Is<T>() && action != null)
                action(self);
            return self;
        }
        public static T IfIam<T>(this T self, [In][Opt] Action<T> action) where T : ITypeClass
        {
            if (self.Is<T>() && action != null)
                action(self);
            return self;
        }
        [return: ReturnMayNull] public static T Fetch<T>(this ITypeClass self, [In][Opt] T data) => data;
        public static T Share<T>(this T self, out T outTy) where T : ITypeClass
        {
            outTy = self;
            return self;
        }
        public static T Share<T>(this T self, [Out][Opt] LeftValueReference<T> outTy)
        {
            if (outTy != null)
                outTy.ref_value = self;
            return self;
        }
    }
    public class TypeClass : ITypeClass
    {
        public override bool Equals(object obj) => base.Equals(obj);
        public override int GetHashCode() => base.GetHashCode();
        public virtual string SymbolName() => this.GetType().Name;

        public static bool operator ==(TypeClass left, Type right) => left.GetType().Equals(right);
        public static bool operator !=(TypeClass left, Type right) => left.GetType().Equals((Type)right) == false;
    }
    public interface IAnyClass : ITypeClass { }
    public class AnyClass : TypeClass, IAnyClass { }
    public class BasicValueReference<T> : AnyClass
    {
        [SerializeField][Content] protected T _ref_value;
        public static implicit operator T(BasicValueReference<T> data) => data._ref_value;

        public override string ToString()
        {
            if (_ref_value == null)
            {
                return $"ref<{typeof(T)}>";
            }
            return _ref_value.ToString();
        }
        public override string SymbolName()
        {
            return $"ref<{typeof(T)}>";
        }
    }
    [Serializable]
    public class LeftValueReference<T> : BasicValueReference<T>
    {
        public LeftValueReference([In][Opt] T ref_value)
        {
            this.ref_value = ref_value;
        }

        public T ref_value { get => _ref_value; set => _ref_value = value; }
        public override string SymbolName()
        {
            return $"{typeof(T)}&";
        }
    }
    public class RightValueReference<T> : BasicValueReference<T> where T : class
    {
        public RightValueReference(T ref_value)
        {
            this.ref_value = ref_value;
        }
        public static implicit operator T(RightValueReference<T> data) => data.ref_value;

        public T ref_value
        {
            get
            {
                T temp = _ref_value;
                _ref_value = null;
                return temp;
            }
            set
            {
                _ref_value = value;
            }
        }
        public override string SymbolName()
        {
            return $"{typeof(T)}&&";
        }
    }

    public class MonoAnyBehaviour : MonoBehaviour, IAnyClass, ConventionEditorInspectorGUI
    {
        [return: ReturnNotNull] public string SymbolName() => this.GetType().Name;
    }

    public class Singleton<T> : MonoAnyBehaviour where T : Singleton<T>
    {
        public static T instance { get; protected set; }

        protected virtual void OnEnable()
        {
            instance = (T)this;
        }
        protected virtual void OnDisable()
        {
            instance = null;
        }
    }
}
