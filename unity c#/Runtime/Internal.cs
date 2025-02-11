using System;
using System.Collections;
using System.Collections.Generic;
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
        protected T _ref_value;
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
    public class LeftValueReference<T> : BasicValueReference<T>
    {
        public LeftValueReference(T ref_value)
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

    [Serializable]
    public class SALCheckException : Exception
    {
        public delegate bool Predicate(object val);
        public Attribute attribute;
        public SALCheckException(Attribute attribute) { this.attribute = attribute; }
        public SALCheckException(Attribute attribute, string message) : base(message) { this.attribute = attribute; }
        public SALCheckException(Attribute attribute, string message, Exception inner) : base(message, inner) { this.attribute = attribute; }
    }
    [System.AttributeUsage(AttributeTargets.Parameter, Inherited = true, AllowMultiple = false)]
    public class InAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.Parameter, Inherited = true, AllowMultiple = false)]
    public class OutAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.Parameter, Inherited = true, AllowMultiple = false)]
    public class OptAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.ReturnValue, Inherited = true, AllowMultiple = false)]
    public class ReturnMayNullAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.ReturnValue, Inherited = true, AllowMultiple = false)]
    public class ReturnNotNullAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.ReturnValue, Inherited = true, AllowMultiple = false)]
    public class ReturnSelfAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.ReturnValue, Inherited = true, AllowMultiple = false)]
    public class ReturnNotSelfAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.ReturnValue, Inherited = true, AllowMultiple = false)]
    public class SucceedAttribute : Attribute
    {
        private SALCheckException.Predicate pr;
        public SucceedAttribute([In] object succeed_when_return_value_is_equal_this_value_or_pr_is_return_true)
        {
            var prm = succeed_when_return_value_is_equal_this_value_or_pr_is_return_true.GetType().GetMethod("Invoke");
            if (prm != null &&
                prm.GetParameters().Length == 1 &&
                prm.ReturnType == typeof(bool))
                this.pr = (SALCheckException.Predicate)succeed_when_return_value_is_equal_this_value_or_pr_is_return_true;
            else
                this.pr = (object obj) => obj == succeed_when_return_value_is_equal_this_value_or_pr_is_return_true;
        }

        public bool Check([In][Opt] object value)
        {
            if (this.pr(value))
                return true;
            throw new SALCheckException(this, $"return value<{value.ToString()[..25]}...> is not expect");
        }
    }
    [System.AttributeUsage(AttributeTargets.ReturnValue, Inherited = true, AllowMultiple = false)]
    public class NotSucceedAttribute : Attribute
    {
        private SALCheckException.Predicate pr;
        public NotSucceedAttribute([In] object failed_when_return_value_is_equal_this_value_or_pr_is_return_true)
        {
            var prm = failed_when_return_value_is_equal_this_value_or_pr_is_return_true.GetType().GetMethod("Invoke");
            if (prm != null &&
                prm.GetParameters().Length == 1 &&
                prm.ReturnType == typeof(bool))
                this.pr = (SALCheckException.Predicate)failed_when_return_value_is_equal_this_value_or_pr_is_return_true;
            else
                this.pr = (object obj) => obj == failed_when_return_value_is_equal_this_value_or_pr_is_return_true;
        }

        public bool Check([In][Opt] object value)
        {
            if (this.pr(value))
                throw new SALCheckException(this, $"return value<{value.ToString()[..25]}...> is not expect");
            return true;
        }
    }
    [System.AttributeUsage(AttributeTargets.Method, Inherited = true, AllowMultiple = false)]
    public class MethodReturnSelfAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.Method, Inherited = true, AllowMultiple = false)]
    public class MethodReturnNotSelfAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.All, Inherited = true, AllowMultiple = true)]
    public class WhenAttribute : Attribute
    {
        private SALCheckException.Predicate pr = null;
        public readonly Type TypenAttribute = null;
        public WhenAttribute([In] object return_value_is_equal_this_value_or_pr_is_return_true, [In] Type typenAttribute)
        {
            this.TypenAttribute = typenAttribute;
            var prm = return_value_is_equal_this_value_or_pr_is_return_true.GetType().GetMethod("Invoke");
            if (prm != null &&
                prm.GetParameters().Length == 1 &&
                prm.ReturnType == typeof(bool))
                this.pr = (SALCheckException.Predicate)return_value_is_equal_this_value_or_pr_is_return_true;
            else
                this.pr = (object obj) => obj == return_value_is_equal_this_value_or_pr_is_return_true;
        }
        public WhenAttribute([In] string description) { }

        public bool CheckAnyValue([In][Opt] object value)
        {
            if (pr == null)
                return true;
            if (this.pr(value))
                return true;
            return false;
        }
    }
    [System.AttributeUsage(AttributeTargets.All, Inherited = false, AllowMultiple = false)]
    public class IgnoreAttribute : Attribute { }
    [System.AttributeUsage(AttributeTargets.All, Inherited = false, AllowMultiple = false)]
    public class SettingAttribute : Attribute { }

    public static partial class ConventionUtility
    {
        public static void InitExtensionEnv()
        {
            UnityExtension.InitExtensionEnv();
        }

        public static bool IsNumber<T>(T data)
        {
            if (data == null) return false;
            var type = data.GetType();
            return
                type == typeof(double) ||
                type == typeof(float) ||
                type == typeof(int) ||
                type == typeof(long) ||
                type == typeof(sbyte) ||
                type == typeof(short) ||
                type == typeof(ushort) ||
                type == typeof(uint) ||
                type == typeof(ulong) ||
                type == typeof(char);
        }
        public static bool IsString<T>(T data)
        {
            if (data == null) return false;
            var type = data.GetType();
            return type == typeof(string) || type == typeof(char[]);
        }
        public static bool IsBinary<T>(T data)
        {
            if (data == null) return false;
            var type = data.GetType();
            return
                type == typeof(byte) ||
                type == typeof(sbyte) ||
                type == typeof(byte[]) ||
                type == typeof(sbyte[]);
        }
        public static bool IsArray<T>(T data)
        {
            if (data == null) return false;
            var type = data.GetType();
            return type.IsArray;
        }
        public static bool IsBool<T>(T data)
        {
            if (data == null) return false;
            return data.GetType() == typeof(bool);
        }
    }
}
