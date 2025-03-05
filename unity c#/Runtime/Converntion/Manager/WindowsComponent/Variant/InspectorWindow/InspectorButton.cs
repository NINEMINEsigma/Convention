using UnityEngine;
using UnityEngine.UI;

namespace Convention.WindowsUI.Variant
{
    public class InspectorButton : InspectorDrawer
    {
        [Resources] public Button RawButton;
        [Resources] public ModernUIButton ModernButton;

        private void OnCallback()
        {
            targetItem.InvokeMember();
        }

        private void Start()
        {
            if (RawButton)
            {
                RawButton.onClick.AddListener(OnCallback);
                if (ModernButton)
                {
                    ModernButton.gameObject.SetActive(false);
                }
            }
            else if (ModernButton)
            {
                ModernButton.AddListener(OnCallback);
            }
        }

        public override void OnInspectorItemInit(InspectorItem item)
        {
            base.OnInspectorItemInit(item);
            if (RawButton)
                RawButton.interactable = item.AbleChangeType;
            if (ModernButton)
                ModernButton.interactable = item.AbleChangeType;
        }

        private void Reset()
        {
            RawButton = GetComponent<Button>();
            ModernButton = GetComponent<ModernUIButton>();
        }
    }
}
