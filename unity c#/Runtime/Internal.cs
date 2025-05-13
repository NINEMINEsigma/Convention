using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
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

        //public static bool operator ==([In] TypeClass left, [In] Type right) => left.GetType().Equals(right);
        //public static bool operator !=([In] TypeClass left, [In] Type right) => left.GetType().Equals(right) == false;
    }
    public interface IAnyClass : ITypeClass { }
    public class AnyClass : TypeClass, IAnyClass { }
    [Serializable]
    public class AnyException : Exception, IAnyClass
    {
        public AnyException() { }
        public AnyException(string message) : base(message) { }
        public AnyException(string message, Exception inner) : base(message, inner) { }
        protected AnyException(
          System.Runtime.Serialization.SerializationInfo info,
          System.Runtime.Serialization.StreamingContext context) : base(info, context) { }

        [return: ReturnNotNull]
        public string SymbolName()
        {
            return $"{this.GetType()}<{base.Message}>";
        }
    }
    public interface ISetRefValue<T>
    {
        T ref_value { get; }
    }
    public interface IGetRefValue<T>
    {
        T ref_value { set; }
    }
    public abstract class BasicValueReference<T> : AnyClass, ISetRefValue<T>, IGetRefValue<T>
    {
        [SerializeField][Content] protected T _ref_value;

        public abstract T ref_value { get; set; }

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

        public override T ref_value { get => _ref_value; set => _ref_value = value; }
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

        public override T ref_value
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

#if UNITY_2017_1_OR_NEWER
    public class MonoAnyBehaviour : MonoBehaviour, IAnyClass, ConventionEditorInspectorGUI
    {
        [return: ReturnNotNull] public string SymbolName() => this.GetType().Name;
    }
#endif

    public interface ISingleton<T> : IAnyClass where T : ISingleton<T> { }

    public static partial class ConventionUtility
    {
        public static T GetSingleton<T>() where T : ISingleton<T>
        {
            return (T)typeof(T).GetProperty("instance", BindingFlags.Static | BindingFlags.Public).GetValue(null);
        }
        public static void SetSingleton<T>(T value) where T : ISingleton<T>
        {
            typeof(T).GetProperty("instance", BindingFlags.Static | BindingFlags.Public).SetValue(null, value);
        }

        public static string nowf()
        {
            return DateTime.Now.ToString("yyyy-MM-dd_HH-mm-ss");
        }
        public static string Nowf => nowf();
    }

    public abstract class Singleton<T> : AnyClass, ISingleton<T> where T : Singleton<T>
    {
        [Setting, Ignore] private static T m_instance;
        public static T instance { get => m_instance; protected set => m_instance = value; }
        public Singleton()
        {
            if (instance != null && instance != this)
                throw new AnyException("instance is exist");
        }
        ~Singleton()
        {
            if (instance == this)
                instance = null;
        }

        public static bool IsAvailable()
        {
            return instance != null;
        }
    }
#if UNITY_2017_1_OR_NEWER
    public abstract class MonoSingleton<T> : MonoAnyBehaviour, ISingleton<T> where T : MonoSingleton<T>
    {

        [Setting, Ignore] private static T m_instance;
        public static T instance { get => m_instance; protected set => m_instance = value; }
        public virtual bool IsDontDestroyOnLoad { get => false; }

        protected virtual void Awake()
        {
            if (instance != null)
            {
                this.gameObject.SetActive(false);
                return;
            }
            if (IsDontDestroyOnLoad && this.transform.parent == null)
                DontDestroyOnLoad(this);
            instance = (T)this;
        }

        public static bool IsAvailable()
        {
            return instance != null;
        }
    }
#endif

    [ArgPackage]
    public class ValueWrapper : AnyClass
    {
        private Func<object> getter;
        private Action<object> setter;
        public readonly Type type;

        public ValueWrapper([In,Opt]Func<object> getter,[In,Opt] Action<object> setter,[In] Type type)
        {
            this.getter = getter;
            this.setter = setter;
            this.type = type;
        }

        public bool IsChangeAble => setter != null;
        public bool IsObtainAble => getter != null;

        public void SetValue(object value)
        {
            setter(value);
        }
        public object GetValue()
        {
            return getter();
        }
    }
}
