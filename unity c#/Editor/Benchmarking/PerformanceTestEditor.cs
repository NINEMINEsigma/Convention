using UnityEditor;
using UnityEngine;
using System;
using UnityEngine.SceneManagement;

namespace Convention
{
    namespace Benchmarking
    {
        [CustomEditor(typeof(PerformanceTestManager))]
        public class PerformanceTestEditor : AbstractCustomEditor
        {
            private void OnAddition()
            {
                PerformanceTestManager performnceTest = (PerformanceTestManager)target;

                //Add current
                if (GUILayout.Button("Add current"))
                {
#if UNITY_URP
                    PerformanceTestStage stage = new PerformanceTestStage();
                    stage.sceneName = SceneManager.GetActiveScene().name;
                    Transform sceneCamTransform = SceneView.lastActiveSceneView.camera.transform;
                    stage.cameraPosition = sceneCamTransform.position;
                    stage.cameraRotation = sceneCamTransform.rotation;
                    performnceTest.StageScenes.Add(stage);
#else
                    throw new InvalidOperationException("URP is not setup");
#endif
                }
            }

            public override void OnOriginGUI()
            {
                base.OnOriginGUI();
                OnAddition();
            }

            public override void OnSettingsGUI()
            {
                base.OnSettingsGUI();
                OnAddition();
            }
        }
    }
}