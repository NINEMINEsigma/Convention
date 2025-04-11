using TMPro;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;
using UnityEngine.UI;

namespace Convention.WindowsUI
{
    public class ModernUIFillBar : WindowUIModule
    {
        // Content
        private float lastPercent;
        [Range(0, 1)] public float currentPercent;
        public float minValue = 0;
        public float maxValue = 100;
        public UnityEvent<float> OnValueChange = new();
        public UnityEvent<float> OnEndChange = new();
        public UnityEvent<float> OnTransValueChange = new();
        public UnityEvent<float> OnEndTransChange = new();

        public float value => (maxValue - minValue) * currentPercent + minValue;
        public float Value => (maxValue - minValue) * currentPercent + minValue;
        // Resources
        public Image loadingBar;
        public TextMeshProUGUI textPercent;
        public TextMeshProUGUI textValue;

        public bool IsLockByScript = true;

        // Settings  
        public bool IsPercent = true;
        public bool IsInt = false;
        public float DragChangeSpeed = 0.5f;

        public void Start()
        {
            var Context = this.GetOrAddComponent<BehaviourContextManager>();
            Context.OnDragEvent = BehaviourContextManager.InitializeContextSingleEvent(Context.OnDragEvent, OnDrag);
            this.currentPercent = loadingBar.fillAmount = this.lastPercent = value;
            textPercent.text = (IsPercent ? currentPercent * 100 : currentPercent).ToString("F2") + (IsPercent ? "%" : "");
            textValue.text = GetValue().ToString("F2");
        }

        public void SetValue(float t)
        {
            SetPerecent((t - minValue) / (maxValue - minValue));
        }

        public void SetPerecent(float t)
        {
            if (IsLockByScript) currentPercent = Mathf.Clamp(t, 0, 1);
            else loadingBar.fillAmount = t;
        }

        public void SetPerecent(float t, float a, float b)
        {
            if (IsLockByScript) currentPercent = Mathf.Clamp(t, 0, 1);
            else loadingBar.fillAmount = t;
            minValue = a;
            maxValue = b;
            IsInt = false;
        }

        public void SetPerecent(float t, int a, int b)
        {
            if (IsLockByScript) currentPercent = Mathf.Clamp(t, 0, 1);
            else loadingBar.fillAmount = t;
            minValue = a;
            maxValue = b;
            IsInt = true;
        }

        public void OnDrag(PointerEventData data)
        {
            if (!IsLockByScript) loadingBar.fillAmount += data.delta.x * Time.deltaTime * DragChangeSpeed;
        }

        private bool IsUpdateAndInvoke = true;
        public void LateUpdate()
        {
            if (IsLockByScript) loadingBar.fillAmount = Mathf.Clamp(currentPercent, 0, 1);
            else currentPercent = loadingBar.fillAmount;

            if (currentPercent == lastPercent)
            {
                if (!IsUpdateAndInvoke)
                {
                    IsUpdateAndInvoke = true;
                    OnEndChange.Invoke(currentPercent);
                    OnEndTransChange.Invoke(Value);
                }
                return;
            }

            IsUpdateAndInvoke = false;
            lastPercent = currentPercent;
            OnValueChange.Invoke(currentPercent);
            OnTransValueChange.Invoke(Value);

            textPercent.text = (IsPercent ? currentPercent * 100 : currentPercent).ToString("F2") + (IsPercent ? "%" : "");
            textValue.text = GetValue().ToString("F2");
        }

        public void UpdateWithInvoke()
        {
            if (IsLockByScript) loadingBar.fillAmount = Mathf.Clamp(currentPercent, 0, 1);
            else currentPercent = loadingBar.fillAmount;

            if (currentPercent == lastPercent)
            {
                if (!IsUpdateAndInvoke)
                {
                    IsUpdateAndInvoke = true;
                }
                return;
            }

            IsUpdateAndInvoke = false;
            lastPercent = currentPercent;

            textPercent.text = currentPercent.ToString("F2") + (IsPercent ? "%" : "");
            textValue.text = GetValue().ToString("F2");
        }

        public float GetValue()
        {
            return IsInt ? (int)value : value;
        }

        public int GetIntValue()
        {
            return (int)value;
        }

        public void Set(float min, float max)
        {
            currentPercent = 0;
            minValue = min;
            maxValue = max;
        }

        public void Set(float percent, float min, float max)
        {
            currentPercent = percent;
            minValue = min;
            maxValue = max;
        }
    }
}

