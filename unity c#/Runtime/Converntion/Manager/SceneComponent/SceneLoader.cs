using UnityEngine;
using UnityEngine.Rendering;
using UnityEngine.SceneManagement;

namespace Convention
{
    /// <summary>
    /// Will Load scene when trigger is entered (could be merged with scene trigger)
    /// </summary>
    public class SceneLoader : MonoAnyBehaviour
    {
        [Tooltip("What scene to load")]
        [Setting]public string SceneName;

#if UNITY_URP
        [SerializeField]
        [Resources]private Volume m_CurrentVolume;

        [SerializeField]
        [Resources]private Volume m_DestinationVolume;
#else
        [SerializeField]
        [Resources] private int m_CurrentVolume;

        [SerializeField]
        [Resources] private int m_DestinationVolume;
#endif

        [Tooltip("Used for cinemachine transition")]
        [Setting,SerializeField] private bool m_SkipLoading;

        [SerializeField] public GameObject ControllPanel;

        public VFX.MutiSceneView.CameraView screen;
        public Transform ReferencePoint;

        public void Start()
        {
            if (!SceneTransitionManager.IsAvailable())
            {
                Destroy(this);
                return;
            }

            if (!m_SkipLoading)
            {
                LoadScene();
            }

        }

        private void OnTriggerEnter(Collider other)
        {
            if (other.CompareTag("Player"))
            {
                EnableScene();
            }
        }

        public void EnableScene()
        {
            SceneTransitionManager.EnableScene(this);
        }

        public void DisableScene()
        {
            SceneTransitionManager.DisableScene(this);
        }

        private void OnTriggerExit(Collider other)
        {
            DisableScene();
        }

        private void LoadScene()
        {
            if (!SceneTransitionManager.IsLoaded(SceneName))
            {
                SceneManager.LoadSceneAsync(SceneName, LoadSceneMode.Additive);
                SceneTransitionManager.NotifySceneLoading();
            }
        }

        public void SetVolumeWeights(float weight)
        {
#if UNITY_URP
            if (m_CurrentVolume != null)
            {
                m_CurrentVolume.weight = weight;
            }

            if (m_DestinationVolume != null)
            {
                m_DestinationVolume.weight = 1 - weight;
            }
#endif
        }
#if UNITY_URP
        public void SetCurrentVolume(Volume volume)
        {
            m_CurrentVolume = volume;
        }

        public Volume GetDestinationVolume()
        {
            return m_DestinationVolume;
        }
#else
        public void SetCurrentVolume(int volume)
        {
            m_CurrentVolume = volume;
        }

        public int GetDestinationVolume()
        {
            return m_DestinationVolume;
        }
#endif
    }
}
