using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using Cinemachine;
using System.Linq;
using UnityEngine.UI;

namespace Convention.WindowsUI.Variant
{
    public class SceneGameWindow : WindowsComponent
    {
        [Resources, SerializeField, OnlyNotNullMode,Header("Bar Button Setting")] private WindowManager m_WindowManager;
        [Resources, SerializeField, OnlyNotNullMode(nameof(m_WindowManager))] private GameObject m_root;
        [Resources, SerializeField, OnlyNotNullMode(nameof(m_WindowManager)), TextArea(1, 3)] private string m_planePath;
        [Resources, SerializeField] private string moduleName = "Game";
        [Resources, SerializeField,Header("Camera Base")] private CinemachineVirtualCameraBase SceneCamera;
        [Resources, SerializeField] private CinemachineVirtualCameraBase ModuleCamera;
        [Resources, SerializeField,HopeNotNull] private CinemachineBrain MainCamera;
        [Resources, SerializeField, OnlyNotNullMode] private RawImage TextureRenderer;

        public BaseWindowBar.RegisteredPageWrapper GameWindowIndex { get; private set; }

        public void CameraSelect(bool isScene)
        {
            if (isScene)
            {
                SceneCamera.gameObject.SetActive(true);
                ModuleCamera.gameObject.SetActive(false);
            }
            else
            {
                SceneCamera.gameObject.SetActive(false);
                ModuleCamera.gameObject.SetActive(true);
            }
        }



        private void Start()
        {
            if (m_WindowManager == null)
            {
                m_WindowManager = GetComponent<WindowManager>();
            }
            if(MainCamera==null)
            {
                MainCamera = Camera.main.GetComponent<CinemachineBrain>();
            }
            CameraInitializer.InitializeImmediate(MainCamera.gameObject);
            TextureRenderer.texture = MainCamera.GetComponent<Camera>().targetTexture;

            if (m_root == null)
            {
                m_root = m_WindowManager.CurrentContextRectTransform.gameObject;
            }
            var root = Instantiate(m_root, m_WindowManager.WindowPlane.Plane.transform);
            var plane = root.transform;
            if (m_planePath != null && m_planePath.Length != 0)
            {
                var paths = m_planePath.Split('/', '\\');
                foreach (var path in paths)
                {
                    var temp= plane.Find(path);
                    if (temp == null)
                        throw new NullReferenceException($"{path} cannt find in {plane}");
                    plane = temp;
                }
            }
            GameWindowIndex = m_WindowManager.CreateSubWindowWithBarButton(plane as RectTransform, root.GetComponent<RectTransform>());
            (GameWindowIndex.button as ITitle).title = moduleName;
            GameWindowIndex.button.AddListener(() =>
            {
                CameraSelect(false);
                GameWindowIndex.Select();
            });
        }
    }
}

