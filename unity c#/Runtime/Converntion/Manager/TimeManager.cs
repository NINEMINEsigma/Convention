using System;
using System.Collections;
using System.Collections.Generic;
using Convention.WindowsUI.Variant;
using UnityEngine;

namespace Convention
{
    public class TimeManager : MonoAnyBehaviour, ISingleton<TimeManager>, IOnlyLoadedInHierarchy
    {
        public delegate void TimeDelegate(float time);

        private RegisterWrapper<TimeManager> m_registerWrapper;
        private static TimeManager m_instance;
        public static TimeManager instance
        {
            get
            {
                if (m_instance == null)
                    m_instance = new GameObject(nameof(TimeManager)).AddComponent<TimeManager>();
                return m_instance;
            }
        }

        [Content] public bool isLoadInHierarchy = true;

        [Content, Ignore, OnlyPlayMode] private TimeInfo m_DefaultClock;
        [OnlyPlayMode, InspectorDraw(InspectorDrawType.Text)]
        public float DefaultClockTime
        {
            get => m_DefaultClock.time;
            set => m_DefaultClock.time = value;
        }
        [OnlyPlayMode, InspectorDraw(InspectorDrawType.Text)]
        public float DefaultClockSpeed
        {
            get => m_DefaultClock.speed;
            set => m_DefaultClock.speed = value;
        }
        [OnlyPlayMode, InspectorDraw(InspectorDrawType.Enum)]
        public TimeInfo.UpdateType DefaultClockUpdateType
        {
            get => m_DefaultClock.updateType;
            set => m_DefaultClock.updateType = value;
        }
        [OnlyPlayMode, InspectorDraw(InspectorDrawType.Enum)]
        public TimeInfo.FocusType DefaultClockFocusType
        {
            get => m_DefaultClock.focusType;
            set => m_DefaultClock.focusType = value;
        }


        private void Start()
        {
            if (m_instance != null)
            {
                Destroy(this.gameObject);
                return;
            }
            m_instance = this;
            GameObject.DontDestroyOnLoad(this.gameObject);
            m_DefaultClock = new TimeInfo()
            {
                name = "",
                updateType = TimeInfo.UpdateType.Update,
                focusType = TimeInfo.FocusType.Scale,
                speed = 1,
                time = 0
            };
            clockers.Add(m_DefaultClock);
            m_registerWrapper = new(() => { });
            if (isLoadInHierarchy)
                gameObject.AddComponent<HierarchyLoadedIn>();
        }
        private void OnDestroy()
        {
            if (m_instance == this)
                m_instance = null;
        }

        [ArgPackage, Serializable]
        public class TimeInfo : AnyClass
        {
            public string name;
            public float time = 0;
            public float speed = 1;
            public UpdateType updateType = UpdateType.Update;
            public FocusType focusType = FocusType.Scale;
            public enum UpdateType
            {
                NotUpdate = 0, Update, FixedUpdate
            }
            public enum FocusType
            {
                Scale, Unscale
            }
        }
        [ArgPackage, Serializable]
        public class TimePointInvoke : AnyClass, IComparable<TimePointInvoke>
        {
            public string listen = "";
            public float enterPoint = 0;
            public float duration = 0;
            public float exitPoint => enterPoint + duration;
            public TimeDelegate callback;

            public int CompareTo(TimePointInvoke other)
            {
                return enterPoint.CompareTo(other.enterPoint);
            }
        }
        [Setting] public List<TimeInfo> clockers = new();
        [Setting] private Dictionary<string, TimeInfo> clockers_D = new();
        [Setting] public float delay = 0;
        [Content, SerializeField, OnlyPlayMode] private bool IsEnableClock = false;
        [Content, SerializeField] private List<TimePointInvoke> AllInvokers = new();
        [Content, Ignore, SerializeField] private Dictionary<string, TimePointInvoke> AllInvokers_D = new();
        private readonly PriorityQueue<TimePointInvoke> FutureInvokers = new();
        private readonly PriorityQueue<TimePointInvoke> OngoingInvokers = new((x, y) => x.exitPoint.CompareTo(y.exitPoint));
        private readonly PriorityQueue<TimePointInvoke> PastInvokers = new();

        [Content, InspectorDraw, OnlyPlayMode]
        public void StartClock()
        {
            IsEnableClock = true;
            AllInvokers.Sort((L, R) => L.enterPoint.CompareTo(R.enterPoint));
            foreach (var clocker in clockers)
            {
                clockers_D.Add(clocker.name, clocker);
            }
        }
        [Content, InspectorDraw, OnlyPlayMode]
        public void StopClock()
        {
            IsEnableClock = false;
            FutureInvokers.Clear();
            OngoingInvokers.Clear();
            PastInvokers.Clear();
            clockers_D.Clear();
        }
        public void AddInvokerByInterval(string listenClockName, TimeDelegate action, params Vector2[] intervals)
        {
            foreach (var interval in intervals)
            {
                var invoker = new TimePointInvoke()
                {
                    listen = listenClockName,
                    enterPoint = interval.x,
                    duration = interval.y - interval.x,
                    callback = action
                };

                AllInvokers.Add(invoker);
                AllInvokers_D.Add(listenClockName, invoker);

                if (IsEnableClock)
                {
                    float currentTime = GetClockTime(listenClockName);
                    if (currentTime < invoker.enterPoint)
                    {
                        FutureInvokers.Enqueue(invoker);
                    }
                    else if (currentTime >= invoker.enterPoint && currentTime < invoker.exitPoint)
                    {
                        OngoingInvokers.Enqueue(invoker);
                    }
                    else
                    {
                        PastInvokers.Enqueue(invoker);
                    }
                }
            }
        }
        public void AddInvokeByDuration(string listenClockName, TimeDelegate action, params Vector2[] intervals)
        {
            foreach (var interval in intervals)
            {
                var invoker = new TimePointInvoke()
                {
                    listen = listenClockName,
                    enterPoint = interval.x,
                    duration = interval.y,
                    callback = action
                };

                AllInvokers.Add(invoker);
                AllInvokers_D.Add(listenClockName, invoker);

                if (IsEnableClock)
                {
                    float currentTime = GetClockTime(listenClockName);
                    if (currentTime < invoker.enterPoint)
                    {
                        FutureInvokers.Enqueue(invoker);
                    }
                    else if (currentTime >= invoker.enterPoint && currentTime < invoker.exitPoint)
                    {
                        OngoingInvokers.Enqueue(invoker);
                    }
                    else
                    {
                        PastInvokers.Enqueue(invoker);
                    }
                }
            }
        }
        public void AddInvoker(TimePointInvoke point)
        {
            AllInvokers.Add(point);
            AllInvokers_D.Add(point.listen, point);

            if (IsEnableClock)
            {
                float currentTime = GetClockTime(point.listen);
                if (currentTime < point.enterPoint)
                {
                    FutureInvokers.Enqueue(point);
                }
                else if (currentTime >= point.enterPoint && currentTime < point.exitPoint)
                {
                    OngoingInvokers.Enqueue(point);
                }
                else
                {
                    PastInvokers.Enqueue(point);
                }
            }
        }
        public void AddInvokers(IEnumerable<TimePointInvoke> points)
        {
            AllInvokers.AddRange(points);
            foreach (var point in points)
            {
                AllInvokers_D.Add(point.listen, point);
            }

            if (IsEnableClock)
            {
                foreach (var point in points)
                {
                    float currentTime = GetClockTime(point.listen);
                    if (currentTime < point.enterPoint)
                    {
                        FutureInvokers.Enqueue(point);
                    }
                    else if (currentTime >= point.enterPoint && currentTime < point.exitPoint)
                    {
                        OngoingInvokers.Enqueue(point);
                    }
                    else
                    {
                        PastInvokers.Enqueue(point);
                    }
                }
            }
        }

        public float GetClockTime(string clockName)
        {
            return clockers_D[clockName].time;
        }
        public float GetClockTime(string clockName, out float time)
        {
            time = clockers_D[clockName].time;
            return time;
        }

        private void Update()
        {
            if (!IsEnableClock) return;
            var dt = Time.deltaTime;
            var unsdt = Time.unscaledDeltaTime;
            foreach (var clocker in clockers)
            {
                if (clocker.updateType == TimeInfo.UpdateType.Update)
                {
                    if (clocker.focusType == TimeInfo.FocusType.Scale)
                        clocker.time += clocker.speed * dt;
                    else
                        clocker.time += clocker.speed * unsdt;
                }
            }
        }

        private void FixedUpdate()
        {
            if (!IsEnableClock) return;
            var dt = Time.fixedDeltaTime;
            foreach (var clocker in clockers)
            {
                if (clocker.updateType == TimeInfo.UpdateType.FixedUpdate)
                {
                    clocker.time += clocker.speed * dt;
                }
            }
        }

        private void LateUpdate()
        {
            if (!IsEnableClock) return;

            // 检查任务的活动情况
            while (FutureInvokers.Count > 0 && FutureInvokers.Peek().enterPoint <= GetClockTime(FutureInvokers.Peek().listen, out float currentTime))
            {
                var point = FutureInvokers.Peek();
                FutureInvokers.Dequeue();

                // 检查任务是否直接从未来变为过去
                if (point.exitPoint <= currentTime)
                {
                    // 任务持续时间过短，直接调用委托并传递结束时间
                    point.callback?.Invoke(point.exitPoint);
                    PastInvokers.Enqueue(point);
                }
                else
                {
                    // 任务进入活动状态
                    OngoingInvokers.Enqueue(point);
                }
            }

            // 检查正在进行的任务
            while (OngoingInvokers.Count > 0 && OngoingInvokers.Peek().exitPoint >= GetClockTime(OngoingInvokers.Peek().listen, out float currentTime))
            {
                OngoingInvokers.Peek().callback(currentTime);
            }
        }
    }
}

