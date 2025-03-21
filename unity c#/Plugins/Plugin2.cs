using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Convention
{
    public static class PluginExtenion
    {
        public static void SelectFileOnSystem(Action<string> action, string labelName, string subLabelName, params string[] fileArgs)
        {
#if PLATFORM_WINDOWS
            WindowsKit.SelectFileOnSystem(action, labelName, subLabelName, fileArgs);
#elif PLATFORM_WEBGL               

#endif
        }
    }

    public class PriorityQueue<T> where T : IComparable<T>
    {
        private int _size;
        private int _capacity;
        private T[] _elements;
        public readonly IComparer<T> _comparer = null;
        public readonly Func<T, T, int> _comparer_func = null;
        public readonly Comparator _comparator = Comparator.less;

        public int Size => _size;
        public int Capacity => _capacity;
        public int Count => _size;
        public bool IsEmpty => _size == 0;
        public T Top => _elements[0];

        public void Clear()
        {
            Array.Clear(_elements, 0, _size);
            _size = 0;
        }

        public PriorityQueue(Comparator comparator = Comparator.less, int capacity = 1)
        {
            _size = 0;
            _capacity = Math.Max(1, capacity);
            _comparator = comparator;
            _elements = new T[_capacity];
        }
        public PriorityQueue(IComparer<T> comparer, int capacity = 1)
        {
            _size = 0;
            _capacity = Math.Max(1, capacity);
            _comparer = comparer;
            _elements = new T[_capacity];
        }
        public PriorityQueue(Func<T, T, int> comparer, int capacity = 1)
        {
            _size = 0;
            _capacity = Math.Max(1, capacity);
            _comparer_func = comparer;
            _elements = new T[_capacity];
        }

        private int Compare(T x, T y)
        {
            if (_comparer != null)
            {
                return _comparer.Compare(x, y) * (int)_comparator;
            }
            else if (_comparer_func != null)
            {
                return _comparer_func(x, y);
            }
            else
            {
                return x.CompareTo(y) * (int)_comparator;
            }
        }

        private void ShiftDown()
        {
            int cur = 0;
            int child = 1;
            while (child < _size)
            {
                if (child + 1 < _size && Compare(_elements[child + 1], _elements[child]) < 0)
                    child++;
                if (Compare(_elements[child], _elements[cur]) < 0)
                {
                    Swap(ref _elements[child], ref _elements[cur]);
                    cur = child;
                    child = 2 * cur + 1;
                }
                else break;
            }
        }

        private void ShiftUp()
        {
            int cur = _size - 1;
            int parent = (cur - 1) / 2;
            while (cur > 0)
            {
                if (Compare(_elements[cur], _elements[parent]) < 0)
                {
                    Swap(ref _elements[cur], ref _elements[parent]);
                    cur = parent;
                    parent = (cur - 1) / 2;
                }
                else break;
            }
        }

        private void ExpandCapacity()
        {
            int newCapacity = Math.Max(_capacity * 2, 4);
            T[] temp = new T[newCapacity];
            Array.Copy(_elements, temp, _size);
            _elements = temp;
            _capacity = newCapacity;
        }

        public void EnsureCapacity(int minCapacity)
        {
            if (_capacity < minCapacity)
            {
                int newCapacity = Math.Max(_capacity * 2, minCapacity);
                T[] temp = new T[newCapacity];
                Array.Copy(_elements, temp, _size);
                _elements = temp;
                _capacity = newCapacity;
            }
        }

        public T Peek()
        {
            if (_size == 0)
                throw new InvalidOperationException("Queue is empty");
            return _elements[0];
        }

        public T Dequeue()
        {
            if (_size == 0)
                throw new InvalidOperationException("Queue is empty");
            
            T result = _elements[0];
            Swap(ref _elements[0], ref _elements[_size - 1]);
            _size--;
            ShiftDown();
            return result;
        }

        public bool TryDequeue(out T result)
        {
            if (_size == 0)
            {
                result = default;
                return false;
            }
            result = Dequeue();
            return true;
        }

        public void Enqueue(T value)
        {
            if (_size == _capacity)
                ExpandCapacity();
            _elements[_size++] = value;
            ShiftUp();
        }

        public bool Contains(T item)
        {
            for (int i = 0; i < _size; i++)
            {
                if (EqualityComparer<T>.Default.Equals(_elements[i], item))
                    return true;
            }
            return false;
        }

        public T[] ToArray()
        {
            T[] result = new T[_size];
            Array.Copy(_elements, result, _size);
            return result;
        }

        public void TrimExcess()
        {
            if (_size < _capacity * 0.9)
            {
                T[] temp = new T[_size];
                Array.Copy(_elements, temp, _size);
                _elements = temp;
                _capacity = _size;
            }
        }

        private void Swap(ref T a, ref T b)
        {
            T temp = a;
            a = b;
            b = temp;
        }

        public enum Comparator
        {
            less = -1,
            equal = 0,
            greater = 1
        }
    }
}
