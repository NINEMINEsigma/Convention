using System.Collections.Generic;
using UnityEngine;

namespace Convention
{
    [RequireComponent(typeof(Camera))]
    public class CameraInitializer : MonoAnyBehaviour
    {
        [Setting, SerializeField] private List<SO.CameraInitializerConfig> Configs = new();

        public void InitializeImmediate()
        {
            var camera = GetComponent<Camera>();
            foreach (var config in Configs)
            {
                config.Invoke(camera);
            }
            DestroyImmediate(this);
        }

        private void Awake()
        {
            InitializeImmediate();
        }

        public static void InitializeImmediate(GameObject target)
        {
            if (target.GetComponents<CameraInitializer>().Length != 0)
            {
                foreach(var initer in target.GetComponents<CameraInitializer>())
                    initer.InitializeImmediate();
            }
        }
    }

    namespace SO
    {
        public abstract class CameraInitializerConfig : ScriptableObject
        {
            public abstract void Invoke(Camera camera);
        }
    }
}