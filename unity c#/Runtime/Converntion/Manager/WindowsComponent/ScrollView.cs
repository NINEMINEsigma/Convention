using UnityEngine;

namespace Convention.WindowsUI
{
    public partial class ScrollView : WindowUIModule
    {
        [Resources, HopeNotNull] public UnityEngine.UI.ScrollRect scrollRect;
        private void Reset()
        {
            scrollRect = GetComponent<UnityEngine.UI.ScrollRect>();
        }
    }
}
