using UnityEngine;

namespace Convention
{
#if UNITY_EDITOR
    public class ExampleHelpWindow : MonoAnyBehaviour
    {
        [Content] public string m_Title;
        [Content, TextArea(minLines: 10, maxLines: 50)] public string m_Description;

        [SerializeField, Setting] private bool mShowingHelpWindow = true;
        [SerializeField, Setting] private float kPadding = 40f;

        private void OnGUI()
        {
            if (mShowingHelpWindow)
            {
                Vector2 size = GUI.skin.label.CalcSize(new GUIContent(m_Description));
                Vector2 halfSize = size * 0.5f;

                float maxWidth = Mathf.Min(Screen.width - kPadding, size.x);
                float left = Screen.width * 0.5f - maxWidth * 0.5f;
                float top = Screen.height * 0.4f - halfSize.y;

                Rect windowRect = new Rect(left, top, maxWidth, size.y);
                GUILayout.Window(400, windowRect, (id) => DrawWindow(id, maxWidth), m_Title);
            }
        }

        private void DrawWindow(int id, float maxWidth)
        {
            GUILayout.BeginVertical(GUI.skin.box);
            GUILayout.Label(m_Description);
            GUILayout.EndVertical();
            if (GUILayout.Button("Got it!"))
            {
                mShowingHelpWindow = false;
            }
        }
    }
#else
    public class ExampleHelpWindow : MonoAnyBehaviour {}
#endif
}
