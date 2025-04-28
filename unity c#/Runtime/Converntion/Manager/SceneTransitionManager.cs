using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering;
#if UNITY_URP
using UnityEngine.Rendering.Universal;
#endif
using UnityEngine.SceneManagement;
using Convention.Benchmarking;
using Cinemachine;
using System.Collections;

namespace Convention
{
    public class SceneTransitionManager : MonoSingleton<SceneTransitionManager>
    {
        private RegisterWrapper<SceneTransitionManager> m_RegisterWrapper;

        public override bool IsDontDestroyOnLoad => true;

        [Tooltip("The amount of time it takes to transition between two scenes")]
        [SerializeField, Setting] private float TransitionDuration;

        [Resources, SerializeField] private Camera m_MainCamera;
        [Resources, SerializeField] private Camera m_ScreenCamera;
        [Resources, SerializeField] private CharacterController m_Player;
        [Resources, SerializeField] private PlayerManager m_CameraManager;

        [Tooltip("Current Scene")]
        [Content, Ignore, SerializeField] private string currentSceneName;
        [Tooltip("Transition Scene")]
        [Content, Ignore, SerializeField] private string targetSceneName;
#if UNITY_URP
        [Tooltip("Layers to render when in a location")]
        [Setting, SerializeField] private SerializedDictionary<string, LayerMask> locationLayers;
        [Tooltip("Current Registered Scenes")]
        [Content,Ignore,SerializeField] private SerializedDictionary<string, SceneMetaData> registeredScenes;
#else
        [Tooltip("Layers to render when in a location")]
        [Setting, SerializeField] private Dictionary<string, LayerMask> locationLayers;
        [Tooltip("Current Registered Scenes")]
        [Content, Ignore, SerializeField] private Dictionary<string, SceneMetaData> registeredScenes;
#endif

        private SceneLoader m_Loader;

        private Transform spawnTransform;

        private int m_ScenesLoading;

        private bool m_ScreenOff;

        protected override void Awake()
        {
            if (PerformanceTestManager.RunningBenchmark)
            {
                Destroy(gameObject);
                return;
            }

            base.Awake();

            SetupReferences();

            SetupInitialState();

            m_RegisterWrapper = new(() => { });
        }

        private void OnDestroy()
        {
            m_RegisterWrapper.Release();
        }

        #region Awake

        public void SetupReferences()
        {
            if (m_Player == null)
                m_Player = FindAnyObjectByType<CharacterController>();

            if (m_CameraManager == null)
                m_CameraManager = FindAnyObjectByType<PlayerManager>();

            if (m_MainCamera == null)
                m_MainCamera = Camera.main;

            if (m_ScreenCamera == null)
            {
                m_ScreenCamera = GameObject.FindGameObjectWithTag("ScreenCamera")?.GetComponent<Camera>();
                if (m_ScreenCamera == null)
                {
                    foreach (var camera in FindObjectsByType<Camera>(FindObjectsSortMode.None))
                    {
                        if (camera != m_MainCamera)
                        {
                            m_ScreenCamera = camera;
                            break;
                        }
                    }
                }
            }
            m_ScreenCamera.GetComponent<Camera>().enabled = false;
        }

        public void SetupInitialState()
        {
            //currentScene init will in first RegisterScene
            InTransition = false;
            registeredScenes = new();
            m_ScreenOff = true;
            ElapsedTimeInTransition = 0;
            RenderSettings.defaultReflectionMode = DefaultReflectionMode.Custom;
        }
        #endregion

#if UNITY_URP
        [SerializeField, Resources] private VFX.FullscreenEffect m_FullscreenEffect;
#endif
        private bool InTransition = false;
        [SerializeField, Content, OnlyPlayMode, Ignore] private bool CoolingOff = false; //After teleporting
        [SerializeField, Content, OnlyPlayMode, Ignore] private float ElapsedTimeInTransition = 0;
        void Update()
        {
            float t = ElapsedTimeInTransition / TransitionDuration;

            if (InTransition)
            {
                ElapsedTimeInTransition += Time.deltaTime;

                if (ElapsedTimeInTransition > TransitionDuration)
                {
                    TriggerTeleport();
                }

                ElapsedTimeInTransition = Mathf.Min(TransitionDuration, ElapsedTimeInTransition);
            }
            else
            {
                ElapsedTimeInTransition -= Time.deltaTime * 3;

                if (ElapsedTimeInTransition < 0 && CoolingOff)
                {
                    CoolingOff = false;
                }

                ElapsedTimeInTransition = Mathf.Max(0, ElapsedTimeInTransition);
            }

            //Update weights of post processing volumes
            if (m_Loader != null && !CoolingOff)
            {
                float tSquared = t * t;
                m_Loader.SetVolumeWeights(1 - tSquared);
            }

#if UNITY_URP
            m_FullscreenEffect.SetEffectWeight(t);
#endif

            void TriggerTeleport()
            {
                InTransition = false;

                if (m_Loader != null)
                {
                    m_Loader.SetVolumeWeights(1);
                }

                //if (m_MediaSceneLoader) //check this some other way
                //{
                //    CinemachineTeleport();
                //}
                //else
                //{
                //    Teleport();
                //}

                CinemachineTeleport();

                m_Loader = null;
                CoolingOff = true;
            }

            void SwapMask()
            {
                var mainLayer = locationLayers[currentSceneName];
                var screenLayer = locationLayers[targetSceneName];
                //Add to mask
                m_MainCamera.cullingMask |= screenLayer;
                m_ScreenCamera.cullingMask |= mainLayer;
                //Remove from mask
                m_MainCamera.cullingMask ^= mainLayer;
                m_ScreenCamera.cullingMask ^= screenLayer;
            }

            void CinemachineTeleport()
            {
                SwapMask();

                //if (instance.IsInStartRegisterScene)
                //{
                //    flythroughRoot.position = instance.m_ScreenCamera.GetComponent<OffsetCamera>().GetOffset();
                //    instance.m_Loader.SetCurrentVolume(instance.m_Loader.GetDestinationVolume());
                //    instance.screenScene.TerminalLoader.SetActive(true);
                //}
                //else
                //{
                //    flythroughRoot.position = Vector3.zero;
                //    instance.m_Loader = instance.m_MediaSceneLoader.GetTerminalSceneLoader();
                //}

                currentSceneName = targetSceneName;
            }

            /*
            void Teleport()
            {
                if (instance.currentSceneName == null || instance.targetSceneName == null)
                {
                    Debug.LogError("Can't teleport without two scenes enabled");
                    return;
                }

               SwapMask();

                //Swap Camera positions
                Transform playerTransform = instance.m_Player.transform;

                //Disable character controller while manipulating positions
                StarterAssets.FirstPersonController controller = playerTransform.GetComponent<StarterAssets.FirstPersonController>();
                controller.enabled = false;

                bool newPositionLocked = instance.registeredScenes[instance.targetSceneName].CameraLockTransform != null;
                bool comingFromLockedPosition = instance.registeredScenes[instance.currentSceneName].CameraLockTransform != null;

                if (newPositionLocked)
                {
                    //Cache transform player before moving
                    instance.m_PositionAtLock = playerTransform.position;
                    instance.m_RotationAtLock = playerTransform.rotation;

                    //Set position, parent and rotation to new locked location
                    Transform cameraLockTransform = instance.screenScene.CameraLockTransform;

                    playerTransform.parent.parent = cameraLockTransform;

                    playerTransform.position = cameraLockTransform.position;
                    playerTransform.rotation = cameraLockTransform.rotation;

                    //Disable the player to prevent them from moving
                    instance.m_Player.enabled = false;

                    instance.m_MainCamera.GetComponent<UniversalAdditionalCameraData>().renderPostProcessing = false; //TODO: this is hardcoded for the cockpit. Should probably be in the metadata

                    DisableScene(instance.currentScene);
                }
                else
                {
                    //Find the offset between the player camera and feet positions
                    Vector3 playerCameraOffset = instance.m_MainCamera.transform.position - instance.m_Player.transform.position;

                    //Position the player at the screen camera position
                    playerTransform.position = instance.m_ScreenCamera.transform.position - playerCameraOffset;

                    //Toggle the offset of the screen camera to put it where the player used to be
                    OffsetCamera oc = instance.m_ScreenCamera.GetComponent<OffsetCamera>();
                    oc.ToggleOffset();

                    //Reset transform if teleporting from a locked position
                    if (comingFromLockedPosition)
                    {

                        Transform playerParent = playerTransform.parent;

                        playerParent.rotation = Quaternion.identity;
                        playerParent.parent = null;
                        DontDestroyOnLoad(playerParent);
                        playerTransform.rotation = instance.m_RotationAtLock;
                        playerTransform.position = instance.m_PositionAtLock;
                        instance.m_Player.enabled = true;

                        EnableScene(instance.screenScene);
                    }

                    instance.m_MainCamera.GetComponent<UniversalAdditionalCameraData>().renderPostProcessing = true; //see same line in the locked transform case
                }

                instance.m_MainCamera.GetComponent<CinemachineBrain>().m_WorldUpOverride =
                    instance.screenScene.WorldUpTransform;

                //Enable or disable post based on what the new scene needs
                UniversalAdditionalCameraData mainCameraData = instance.m_MainCamera.GetComponent<UniversalAdditionalCameraData>();

                mainCameraData.renderPostProcessing = instance.screenScene.PostProcessingEnabled;
                mainCameraData.SetRenderer(instance.screenScene.RendererIndex);

                //Reenable controller after teleporting
                instance.m_EneableCharacterInFollowingFrame = true;

                SceneManager.SetActiveScene(instance.screenScene.Scene);

                //This is weird
                RenderSettings.defaultReflectionMode = DefaultReflectionMode.Custom;



                //Swap references to screen and current scene
                (instance.screenScene, instance.currentScene) = (instance.currentScene, instance.screenScene);

                //Setup terminal loader so player can get back and reset the timeline director
                instance.SetTerminalLoaderAndDirector(instance.screenScene, false);
                instance.SetTerminalLoaderAndDirector(instance.currentScene, true);
            }
            */
        }

        /// <summary>
        /// This function is called per camera by the render pipeline.
        /// We use it to set up light and render settings (skybox etc) for the different scenes as they are displayed
        /// </summary>
        void OnBeginCameraRendering(ScriptableRenderContext context, Camera camera)
        {
            bool isMainCamera = camera.CompareTag("MainCamera");

            if (!isMainCamera && targetSceneName == null)
            {
                //If no screen scene is loaded, no setup needs to be done for it
                return;
            }

            //Toggle main light
            if (camera.cameraType == CameraType.SceneView)
            {
                ToggleMainLight(registeredScenes[currentSceneName], true);
                ToggleMainLight(registeredScenes[targetSceneName], false);
            }
            else
            {
                ToggleMainLight(registeredScenes[currentSceneName], isMainCamera);
                ToggleMainLight(registeredScenes[targetSceneName], !isMainCamera);
            }

            //Setup render settings
            SceneMetaData sceneToRender = registeredScenes[isMainCamera || camera.cameraType == CameraType.SceneView ? currentSceneName : targetSceneName];
            RenderSettings.fog = sceneToRender.FogEnabled;
            RenderSettings.skybox = sceneToRender.skybox;
            if (sceneToRender.reflection != null)
            {
                RenderSettings.customReflectionTexture = sceneToRender.reflection;
            }

            if (!isMainCamera && camera.cameraType == CameraType.Game)
            {
                camera.GetComponent<OffsetCamera>().UpdateWithOffset();
            }

            static void ToggleMainLight(SceneMetaData scene, bool value)
            {
                if (scene != null && scene.mainLight != null)
                {
                    scene.mainLight.SetActive(value);
                }
            }
        }

        #region On Enable/Disable

        private void OnEnable()
        {
            RenderPipelineManager.beginCameraRendering += OnBeginCameraRendering;
            SceneManager.sceneLoaded += OnSceneLoaded;
        }

        private void OnDisable()
        {
            RenderPipelineManager.beginCameraRendering -= OnBeginCameraRendering;
            SceneManager.sceneLoaded -= OnSceneLoaded;
        }

        #endregion

        #region Scene Loading

        /// <summary>
        /// This function is called by the metadata script to notify of its existence
        /// </summary>
        public static void RegisterScene([In] string name, [In] SceneMetaData metaData)
        {
            instance.registeredScenes.Add(name, metaData);

            if (instance.currentSceneName == null) //First loaded scene get's assigned to current
            {
                instance.currentSceneName = metaData.Scene.name;
            }
        }

        public static void NotifySceneLoading()
        {
            instance.m_ScenesLoading++;
            //instance.m_Player.GetComponent<StarterAssets.FirstPersonController>().enabled = false;
        }

        void OnSceneLoaded(Scene scene, LoadSceneMode mode)
        {
            instance.m_ScenesLoading--;
            //if (instance.m_ScenesLoading == 0) instance.m_Player.GetComponent<StarterAssets.FirstPersonController>().enabled = true;
        }

        /// <summary>
        /// This function is called by the scene loader when the player enters its trigger
        /// </summary>
        public static void EnableScene(SceneLoader sceneLoader)
        {
            if (!IsAvailable()) return;

            SceneMetaData sceneMetaData = instance.registeredScenes[sceneLoader.SceneName];

            if (sceneMetaData == null)
            {
                throw new Exception("Trying to enable unregistered scene");
            }

            Debug.Log("Enabling this scene: " + sceneMetaData.Scene.name);

            instance.m_Loader = sceneLoader;
            instance.registeredScenes.Add(sceneMetaData.Scene.name, sceneMetaData);
            //instance.currentScene.TerminalLoader = sceneLoader.ControllPanel;

            LightProbes.TetrahedralizeAsync();

            //Enable game objects
            sceneMetaData.EnvironmentGameObjectRoot.SetActive(true);

            //Set the offset of the screen camera 
            if (sceneMetaData.SpawnTransform != null)
            {
                instance.m_ScreenCamera.GetComponent<OffsetCamera>().SetOffset(
                    sceneMetaData.SpawnTransform.position - instance.m_Loader.ReferencePoint.position);
            }

            //Switch on the screens
            if (sceneLoader.screen != null)
            {
                sceneLoader.screen.LoadTextureView(GetScreenRT());
            }

            //Set the renderer index
            int index = sceneMetaData.RendererIndex > 1 ? sceneMetaData.RendererIndex : 1;
#if UNITY_URP
            instance.m_ScreenCamera.GetComponent<UniversalAdditionalCameraData>().SetRenderer(index);
#endif

            instance.m_ScreenCamera.GetComponent<Camera>().enabled = true;
            instance.m_ScreenOff = false;
        }

        public static void EnableScene(SceneMetaData sceneMetaData)
        {
            sceneMetaData.EnvironmentGameObjectRoot.SetActive(true);
            instance.m_ScreenCamera.GetComponent<Camera>().enabled = true;
        }


        public static void DisableScene(SceneLoader sceneLoader)
        {
            if (!IsAvailable()) return;
            SceneMetaData sceneMetaData = instance.registeredScenes[sceneLoader.SceneName];
            if (sceneMetaData.Scene.name == instance.currentSceneName)
            {
                Debug.Log("Trying to disable current scene");
                return;
            }

            Debug.Log("Disabling this scene: " + sceneMetaData.Scene.name);

            LightProbes.TetrahedralizeAsync();

            //Turn off the screen and disable the root object in the scene once screen is completely shut off
            if (sceneLoader.screen != null)
            {
                sceneLoader.screen.UnloadTextureView();
                IEnumerator _Callback()
                {
                    yield return new WaitForSeconds(sceneLoader.screen.Duration);
                    if (instance.m_ScreenOff)
                    {
                        sceneMetaData.EnvironmentGameObjectRoot.SetActive(false);
                        instance.m_ScreenCamera.GetComponent<Camera>().enabled = false;
                    }
                }
                sceneLoader.StartCoroutine(_Callback());
            }

            instance.m_ScreenOff = true;
        }

        public static void DisableScene(SceneMetaData sceneMetaData)
        {
            sceneMetaData.EnvironmentGameObjectRoot.SetActive(false);
            instance.m_ScreenCamera.GetComponent<Camera>().enabled = false;
        }

        public static void DisableLoadedScene()
        {
            if (instance.m_Loader == null) return;
            DisableScene(instance.m_Loader);
        }

        public static void StartTransition()
        {
            instance.InTransition = true;
        }

        public static void StopTransition()
        {
            instance.InTransition = false;
        }

        public static bool DissolveNeeded()
        {
            return instance != null && instance.ElapsedTimeInTransition > 0.001f;
        }

#endregion

        #region Getters

        public static bool IsLoaded(string sceneName)
        {
            return instance.registeredScenes.ContainsKey(sceneName);
        }

        public static RenderTexture GetScreenRT()
        {
            return instance.m_ScreenCamera.activeTexture;
        }

        public static GameObject GetMainCamera()
        {
            return instance.m_MainCamera.gameObject;
        }

        public static SceneMetaData GetCurrentSceneData()
        {
            return instance.registeredScenes[instance.currentSceneName];
        }

        #endregion
    }
}
