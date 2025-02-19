using UnityEditor;
using UnityEngine;
using UnityEngine.SceneManagement;

namespace Convention
{
    namespace Benchmarking
    {
        [CustomEditor(typeof(PerformanceTest))]
        public class PerformanceTestEditor : AbstractCustomEditor
        {
            private void OnAddition()
            {
                PerformanceTest performnceTest = (PerformanceTest)target;

                //Add current
                if (GUILayout.Button("Add current"))
                {
                    PerformanceTestStage stage = new PerformanceTestStage();
                    stage.sceneName = SceneManager.GetActiveScene().name;
                    Transform sceneCamTransform = SceneView.lastActiveSceneView.camera.transform;
                    stage.cameraPosition = sceneCamTransform.position;
                    stage.cameraRotation = sceneCamTransform.rotation;
                    performnceTest._stages.Add(stage);
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