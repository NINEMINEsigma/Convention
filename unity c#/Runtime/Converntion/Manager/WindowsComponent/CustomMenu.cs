using System.Collections.Generic;
using Convention.WindowsUI;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.UI;

namespace Convention
{
    public class CustomMenu : WindowsComponent
    {
        [Setting, SerializeField] private bool IsDestroy = true;
        [Resources, SerializeField, OnlyNotNullMode] private Button ButtonPrefab;
        [Resources, SerializeField, OnlyNotNullMode] private RectTransform Plane;
        [Content] public List<GameObject> childs = new();

        [Content, OnlyPlayMode]
        [return: IsInstantiated(true)]
        public virtual Button CreateItem()
        {
            var item = GameObject.Instantiate(ButtonPrefab, Plane).GetComponent<Button>();
            childs.Add(item.gameObject);
            return item;
        }
        [return: IsInstantiated(true)]
        public virtual Button CreateItem(UnityAction callback, string title)
        {
            var item = CreateItem();
            item.onClick.AddListener(callback);
            item.GetComponents<IText>()[0].text = title;
            return item;
        }
        public virtual void ClearAllItem()
        {
            foreach (var child in childs)
            {
                GameObject.Destroy(child);
            }
            childs.Clear();
        }
        public virtual void ReleaseMenu()
        {
            if (IsDestroy)
                Destroy(this.gameObject);
            else
                gameObject.SetActive(false);
        }
    }
}
