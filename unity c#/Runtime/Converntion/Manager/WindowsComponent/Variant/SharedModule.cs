using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

namespace Convention.WindowsUI.Variant
{
    public class SharedModule : MonoSingleton<SharedModule>, IWindowUIModule
    {
        [Resources, OnlyNotNullMode, SerializeField] private ModernUIInputField RenameField;
        [Resources, OnlyNotNullMode, SerializeField, IsInstantiated(false)] private CustomMenu CustomMenuPrefab;
        [Resources, OnlyNotNullMode, SerializeField, WhenAttribute.Not(nameof(CustomMenuPrefab), null)] private RectTransform CustomMenuPlane;
        [Resources, OnlyNotNullMode, SerializeField, WhenAttribute.Not(nameof(CustomMenuPrefab), null)] private Button CustomMenuRelease;
        [Content,SerializeField,OnlyPlayMode]private List<CustomMenu> customMenus = new List<CustomMenu>();
        private Action<string> RenameCallback;

        private void Start()
        {
            RenameField.AddListener(x =>
            {
                RenameCallback(x);
                RenameField.gameObject.SetActive(false);
            });
            this.CustomMenuRelease.onClick.AddListener(() =>
            {
                ReleaseAllCustomMenu();
            });
        }

        private void ReleaseAllCustomMenu()
        {
            foreach (var menu in customMenus)
            {
                menu.ReleaseMenu();
            }
            CustomMenuRelease.gameObject.SetActive(false);
        }

        public void Rename([In]string initText, [In]Action<string> callback)
        {
            RenameField.gameObject.SetActive(true);
            RenameField.text = initText;
            RenameCallback = callback;
        }

        /// <summary>
        /// 回调函数的参数是root
        /// </summary>
        [return: ReturnNotNull, IsInstantiated(true)]
        public CustomMenu OpenCustomMenu([In] RectTransform root, [In] List<Tuple<string, Action<GameObject>>> actions)
        {
            var target = GameObject.Instantiate(CustomMenuPrefab.gameObject, CustomMenuPlane).GetComponent<CustomMenu>();
            customMenus.Add(target);
            foreach (var action in actions)
            {
                target.CreateItem(() =>
                {
                    action.Item2(root.gameObject);
                    ReleaseAllCustomMenu();
                }, action.Item1);
            }
            Vector3[] points = new Vector3[4];
            root.GetWorldCorners(points);
            var rightTop = points[2];
            Vector3[] points2 = new Vector3[4];
            target.rectTransform.GetWorldCorners(points2);
            var leftTop = points2[1];
            target.rectTransform.Translate(rightTop - leftTop, Space.World);
            CustomMenuRelease.gameObject.SetActive(true);
            return target;
        }
    }
}
