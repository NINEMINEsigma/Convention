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

        private RegisterWrapper<PlayerManager> m_RegisterWrapper;

        void Start()
        {
            m_RegisterWrapper = new(() => { });

            if (PerformanceTestManager.RunningBenchmark)
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
            m_RegisterWrapper.Release();
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
