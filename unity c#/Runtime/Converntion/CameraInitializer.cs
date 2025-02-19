using System.Collections.Generic;
using UnityEngine;

namespace Convention
{
    [RequireComponent(typeof(Camera))]
    public class CameraInitializer : MonoAnyBehaviour
    {
        [Setting, SerializeField] private List<SO.CameraInitializerConfig> Configs = new();

        void Awake()
        {
            var camera = GetComponent<Camera>();
            foreach (var config in Configs)
            {
                config.Invoke(camera);
            }
            Destroy(this);
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