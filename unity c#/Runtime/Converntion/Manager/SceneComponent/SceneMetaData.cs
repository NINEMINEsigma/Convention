using Convention.Benchmarking;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Playables;
using UnityEngine.Rendering;
using UnityEngine.SceneManagement;

namespace Convention
{
    /// <summary>
    /// This script has metadata needed for multi scene rendering and teleporting
    /// It also registers the scene in the scene transition manager
    /// </summary>
    public class SceneMetaData : MonoAnyBehaviour
    {
        [Setting] public GameObject mainLight = null;
        [Setting] public Material skybox = null;
        [Setting] public Cubemap reflection = null;
        [Tooltip("This is used if the scene loaded needs the player to be locked in place")]
        [Setting] public Transform CameraLockTransform = null;
        [Setting] public Transform WorldUpTransform = null;
        [Setting] public Transform SpawnTransform;
        [Header("When Setup, other SceneMetaData.EnvironmentGameObjectRoot will SetActive(false)")]
        [Setting] public GameObject EnvironmentGameObjectRoot;
        [Setting] public Scene Scene;
        [Setting] public bool FogEnabled;
        [Setting] public bool PostProcessingEnabled;
        [Setting] public bool StartActive;
        [Setting] public int RendererIndex = 0;

        void Start()
        {
            if (SceneTransitionManager.IsAvailable() && !PerformanceTestManager.RunningBenchmark)
            {
                SetUp();
            }
        }

        private void SetUp()
        {
            Scene = gameObject.scene;

            GameObject thisParent = this.gameObject;
            while (thisParent.transform.parent != null)
            {
                thisParent = thisParent.transform.parent.gameObject;
            }
            //Disable objects that shouldn't be used in a multi scene setup
            foreach (var go in Scene.GetRootGameObjects())
            {
                if (go != thisParent && !(go == EnvironmentGameObjectRoot && StartActive))
                {
                    go.SetActive(false);
                }
            }

            //Register scene
            new RegisterWrapper<SceneMetaData>(() => SceneTransitionManager.RegisterScene(Scene.name, this));
        }
    }
}
