using Cinemachine;
using UnityEngine;
using UnityEngine.Playables;
using UnityEngine.Timeline;
using Convention.Benchmarking;
using UnityEngine.EventSystems;

namespace Convention
{
    /// <summary>
    /// This class will enable the touch input canvas on handheld devices and will trigger the camera flythrough if the player is idle
    /// </summary>
    public class PlayerManager : MonoAnyBehaviour
    {
        [SerializeField, Resources] private GameObject m_CrosshairCanvas;
        [SerializeField,Resources] private GameObject m_TouchInputCanvas;

        [SerializeField,Resources]private CinemachineVirtualCamera m_VirtualCamera;

        void Start()
        {
            if (PerformanceTest.RunningBenchmark)
            {
                Destroy(gameObject);
                return;
            }

            if (SystemInfo.deviceType == DeviceType.Handheld)
            {
                m_TouchInputCanvas.SetActive(true);
            }

            if (m_VirtualCamera == null)
            {
                m_VirtualCamera = GetComponentInChildren<CinemachineVirtualCamera>();
            }
        }

        public void EnableFirstPersonController()
        {
            if (SceneTransitionManager.IsAvailable())
            {
                SceneTransitionManager.DisableLoadedScene();
                SceneTransitionManager.StopTransition();
            }

            m_VirtualCamera.gameObject.SetActive(true);
            m_CrosshairCanvas.SetActive(true);

        }

        //public void NotifyPlayerMoved()
        //{
        //    m_TimeIdle = 0;
        //    if (m_InFlythrough)
        //    {
        //        EnableFirstPersonController();
        //        if (SystemInfo.deviceType == DeviceType.Handheld)
        //        {
        //            m_TouchInputCanvas.SetActive(true);
        //        }
        //    }
        //}
    }
}
