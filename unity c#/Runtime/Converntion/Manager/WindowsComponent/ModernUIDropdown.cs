using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using TMPro;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using static Convention.ProjectContextLabelAttribute;

namespace Convention.WindowsUI
{
    public class ModernUIDropdown : WindowUIModule, IActionInvoke<string>, ITitle
    {
        // Resources
        [Resources, OnlyNotNullMode] public GameObject triggerObject;
        [Resources,OnlyNotNullMode] public Transform itemParent;
        [Resources,OnlyNotNullMode] public GameObject itemPrefab;
        [Resources, OnlyNotNullMode] public GameObject scrollbar;
        private VerticalLayoutGroup itemList;
        private Transform currentListParent;
        [Resources, WhenAttribute.Is(nameof(isListItem), true)] public Transform listParent;
        private Animator dropdownAnimator;
        [Resources, OnlyNotNullMode] public TextMeshProUGUI m_Title;
        private Button m_RawButton;

        // Settings
        [Setting] public bool enableTrigger = true;
        [Setting] public bool enableScrollbar = true;
        [Setting] public bool setHighPriorty = true;
        [Setting] public bool outOnPointerExit = false;
        [Setting] public bool isListItem = false;
        [Setting] public AnimationType animationType;
        [Setting] public UnityEvent<string> OnSelect = new();
        [Setting] public bool isMutiSelect = false;

        // Items
        [Content] public List<Item> dropdownItems = new();

        // Other variables
        bool isOn;
        [Content] public int siblingIndex = 0;

        private bool m_interactable = true;
        public bool interactable
        {
            get => m_interactable;
            set
            {
                if (m_interactable != value)
                {
                    m_interactable = value;
                    RefreshImmediate();
                }
            }
        }
        public string title { get => m_Title.text; set => m_Title.text = value; }

        public enum AnimationType
        {
            FADING,
            SLIDING,
            STYLISH
        }

        [System.Serializable]
        public class Item
        {
            public string itemName = "Dropdown Item";
            [HideInInspector] private Toggle m_ToggleItem;
            public Toggle ToggleItem
            {
                get => m_ToggleItem;
                set
                {
                    if (m_ToggleItem != value)
                    {
                        m_ToggleItem = value;
                        if (m_ToggleItem != null)
                            m_ToggleItem.isOn = lazy_isOn;
                        lazy_isOn = false;
                    }
                }
            }
            public UnityEvent<bool> toggleEvents = new();
            private bool lazy_isOn = false;
            public bool isOn
            {
                get => ToggleItem == null ? lazy_isOn : ToggleItem.isOn;
                set
                {
                    if (ToggleItem == null)
                        lazy_isOn = value;
                    else
                        ToggleItem.SetIsOnWithoutNotify(value);
                }
            }
        }

        private void Reset()
        {
            triggerObject = transform.Find("Trigger").gameObject;
        }

        [Resources] public RectTransform ResizeBroadcastRect;
        public IEnumerator ResizeBroadcast()
        {
            for (int i = 0; i < 60; i++)
            {
                RectTransformExtension.AdjustSizeToContainsChilds(ResizeBroadcastRect);
                yield return null;
            }
        }

        private void Start()
        {
            m_RawButton = GetComponent<Button>();

            try
            {
                dropdownAnimator = this.GetComponent<Animator>();
                itemList = itemParent.GetComponent<VerticalLayoutGroup>();
                RefreshImmediate();
                currentListParent = transform.parent;
            }

            catch
            {
                DebugError("Dropdown", "Cannot initalize the object", ContextLabelType.Resources, this);
            }

            if (enableScrollbar == true)
            {
                itemList.padding.right = 25;
                scrollbar.SetActive(true);
            }

            else
            {
                itemList.padding.right = 8;
                Destroy(scrollbar);
            }

            if (setHighPriorty == true)
                transform.SetAsLastSibling();
        }

        public void RefreshImmediate()
        {
            foreach (Transform child in itemParent)
                if (child.gameObject != itemPrefab)
                    GameObject.Destroy(child.gameObject);

            for (int i = 0; i < dropdownItems.Count; ++i)
            {
                Item current = dropdownItems[i];

                GameObject go = Instantiate(itemPrefab, itemParent);
                go.SetActive(true);

                go.GetComponentInChildren<TMP_Text>().text = current.itemName;

                Toggle itemToggle = go.GetComponent<Toggle>();

                current.ToggleItem = itemToggle;

                itemToggle.isOn = false;

                itemToggle.interactable = interactable;

            }
            foreach (var current in dropdownItems)
            {
                var itemToggle = current.ToggleItem;
                itemToggle.onValueChanged.AddListener(GenerateCallback(current));
            }

            currentListParent = transform.parent;

            UnityAction<bool> GenerateCallback(Item current)
            {
                void Callback(bool T)
                {
                    if (isMutiSelect)
                    {
                        int selectCount = 0;
                        Item selectedLast = null;
                        foreach (var item in dropdownItems)
                        {
                            if (item.isOn)
                            {
                                selectCount++;
                                selectedLast = item;
                            }
                        }
                        if (selectCount == 0)
                            this.title = "Empty";
                        else if (selectCount == 1)
                            this.title = selectedLast.itemName;
                        else if (selectCount == dropdownItems.Count)
                            this.title = "Every";
                        else
                            this.title = "Muti";
                    }
                    else
                    {
                        if (T)
                        {
                            foreach (var item in dropdownItems)
                            {
                                if (current != item)
                                    item.isOn = false;
                            }
                            this.title = current.itemName;
                        }
                        else
                        {
                            dropdownItems[0].isOn = true;
                            this.title = dropdownItems[0].itemName;
                        }
                    }
                    current.toggleEvents.Invoke(T);
                    OnSelect.Invoke(current.itemName);

                }
                return Callback;
            }
        }

        public void Animate()
        {
            if (isOn == false && animationType == AnimationType.FADING)
            {
                dropdownAnimator.Play("Fading In");
                isOn = true;

                if (isListItem == true)
                {
                    siblingIndex = transform.GetSiblingIndex();
                    gameObject.transform.SetParent(listParent, true);
                }
            }

            else if (isOn == true && animationType == AnimationType.FADING)
            {
                dropdownAnimator.Play("Fading Out");
                isOn = false;

                if (isListItem == true)
                {
                    gameObject.transform.SetParent(currentListParent, true);
                    gameObject.transform.SetSiblingIndex(siblingIndex);
                }
            }

            else if (isOn == false && animationType == AnimationType.SLIDING)
            {
                dropdownAnimator.Play("Sliding In");
                isOn = true;

                if (isListItem == true)
                {
                    siblingIndex = transform.GetSiblingIndex();
                    gameObject.transform.SetParent(listParent, true);
                }
            }

            else if (isOn == true && animationType == AnimationType.SLIDING)
            {
                dropdownAnimator.Play("Sliding Out");
                isOn = false;

                if (isListItem == true)
                {
                    gameObject.transform.SetParent(currentListParent, true);
                    gameObject.transform.SetSiblingIndex(siblingIndex);
                }
            }

            else if (isOn == false && animationType == AnimationType.STYLISH)
            {
                dropdownAnimator.Play("Stylish In");
                isOn = true;

                if (isListItem == true)
                {
                    siblingIndex = transform.GetSiblingIndex();
                    gameObject.transform.SetParent(listParent, true);
                }
            }

            else if (isOn == true && animationType == AnimationType.STYLISH)
            {
                dropdownAnimator.Play("Stylish Out");
                isOn = false;
                if (isListItem == true)
                {
                    gameObject.transform.SetParent(currentListParent, true);
                    gameObject.transform.SetSiblingIndex(siblingIndex);
                }
            }

            StopCoroutine(nameof(ResizeBroadcast));
            StartCoroutine(nameof(ResizeBroadcast));

            if (enableTrigger == true && isOn == false)
                triggerObject.SetActive(false);

            else if (enableTrigger == true && isOn == true)
                triggerObject.SetActive(true);

            if (outOnPointerExit == true)
                triggerObject.SetActive(false);

            if (setHighPriorty == true)
                transform.SetAsLastSibling();
        }

        public void OnPointerExit(PointerEventData eventData)
        {
            if (outOnPointerExit == true)
            {
                if (isOn == true)
                {
                    Animate();
                    isOn = false;
                }

                if (isListItem == true)
                    gameObject.transform.SetParent(currentListParent, true);
            }
        }

        public void UpdateValues()
        {
            if (enableScrollbar == true)
            {
                itemList.padding.right = 25;
                scrollbar.SetActive(true);
            }

            else
            {
                itemList.padding.right = 8;
                scrollbar.SetActive(false);
            }
        }

        public Item CreateOption(string name, params UnityAction<bool>[] actions)
        {
            Item item = new()
            {
                itemName = name,
                toggleEvents = ConventionUtility.WrapperAction2Event(actions)
            };
            dropdownItems.Add(item);
            return item;
        }
        public void RemoveOption(params Item[] items)
        {
            foreach (var item in items)
            {
                dropdownItems.RemoveAll(T => T == item);
            }
            RefreshImmediate();
        }

        public void ClearOptions()
        {
            dropdownItems.Clear();
            RefreshImmediate();
        }

        public void Select(string option)
        {
            var target = dropdownItems.FirstOrDefault(T => T.itemName == option);
            if (target != default)
            {
                target.ToggleItem.isOn = true;
            }
        }

        public IActionInvoke<string> AddListener(params UnityAction<string>[] action)
        {
            foreach (var item in action)
            {
                OnSelect.AddListener(item);
            }
            return this;
        }
        public IActionInvoke<string> RemoveListener(params UnityAction<string>[] action)
        {
            foreach (var item in action)
            {
                OnSelect.RemoveListener(item);
            }
            return this;
        }
        public IActionInvoke<string> RemoveAllListeners()
        {
            OnSelect.RemoveAllListeners();
            return this;
        }
    }
}
