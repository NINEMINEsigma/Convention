using UnityEngine;
using UnityEngine.UI;
using System;

namespace Convention.WindowsUI.Variant
{
    public class InspectorImage : InspectorDrawer
    {
        [Resources] public RawImage ImageArea;
        [Resources] public Button RawButton;

        public void SetImage([In]Texture texture)
        {
            if (targetItem.GetValueType() == texture.GetType())
                targetItem.SetValue(texture);
            else if (targetItem.GetValueType() == typeof(Sprite))
            {
                targetItem.SetValue(texture.CopyTexture().ToSprite());
            }
            else if(targetItem.GetType() == typeof(Texture2D))
            {
                targetItem.SetValue(texture.CopyTexture());
            }
            else
            {
                throw new NotSupportedException("Unsupport Image Convert");
            }
            ImageArea.texture = texture;
        }

        private void OnCallback()
        {
            WindowsKit.SelectFileOnSystem(path =>
            {
                if (path == null || path.Length == 0)
                    return;
                var file = new ToolFile(path);
                if (file.IsExist == false)
                    return;
                Texture2D texture = file.LoadAsImage();
                SetImage(texture);
                if (targetItem.target is IInspectorUpdater updater)
                {
                    updater.OnInspectorUpdate();
                }
            }, "image", "texture", ToolFile.ImageFileExtension);
        }

        private void Start()
        {
            RawButton.onClick.AddListener(OnCallback);
        }

        private void OnEnable()
        {
            RawButton.interactable = targetItem.AbleChangeType;
            if (targetItem.GetValueType().IsSubclassOf(typeof(Texture)))
                ImageArea.texture = (Texture)targetItem.GetValue();
            else if (targetItem.GetValueType() == typeof(Sprite))
                ImageArea.texture = ((Sprite)targetItem.GetValue()).texture;
        }

        private void FixedUpdate()
        {
            if (targetItem.UpdateType && targetItem.GetValueType().IsSubclassOf(typeof(Texture)))
            {
                ImageArea.texture = (Texture)targetItem.GetValue();
            }
        }

        private void Reset()
        {
            ImageArea = GetComponent<RawImage>();
            RawButton = GetComponent<Button>();
        }
    }
}
