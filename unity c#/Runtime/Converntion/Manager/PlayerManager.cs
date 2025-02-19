using Cinemachine;
using Convention.Benchmarking;
using UnityEngine;

namespace Convention
{
    public class PlayerManager : MonoAnyBehaviour
    {
        [SerializeField, Resources] private GameObject m_CrosshairCanvas;
        [SerializeField, Resources] private GameObject m_TouchInputCanvas;

        [SerializeField, Resources] private CinemachineVirtualCamera m_VirtualCamera;

        private RegisterWrapper<PlayerManager> m_register;

        void Start()
        {
            m_register = new(() => { });

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
        private void OnDestroy()
        {
            m_register = null;
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
    }
}
