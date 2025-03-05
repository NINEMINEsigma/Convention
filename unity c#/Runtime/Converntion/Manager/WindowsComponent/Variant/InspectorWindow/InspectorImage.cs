using UnityEngine;
using UnityEngine.UI;

namespace Convention.WindowsUI.Variant
{
    public class InspectorImage : InspectorDrawer
    {
        [Resources] public RawImage ImageArea;
        [Resources] public Button RawButton;

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
                targetItem.SetValue(texture);
                ImageArea.texture = texture;
            }, "image", "texture", ToolFile.ImageFileExtension);
        }

        private void Start()
        {
            RawButton.onClick.AddListener(OnCallback);
        }

        public override void OnInspectorItemInit(InspectorItem item)
        {
            base.OnInspectorItemInit(item);
            RawButton.interactable = item.AbleChangeType;
            ImageArea.texture = (Texture)targetItem.GetValue();
        }

        private void Update()
        {
            if (targetItem.UpdateType)
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
