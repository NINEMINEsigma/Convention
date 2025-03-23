using UnityEngine;

namespace Convention.VFX
{
    public class LinePoint : MonoAnyBehaviour, ILoadedInHierarchy
    {
        public float Scale { get; private set; }
        public float ScaleOne = new Vector3(1, 1, 1).magnitude;
        public Vector3 Forward { get; private set; }
        public Color PointColor = Color.white;
        public float PointWeight = 1;

        private void Update()
        {
            Scale = transform.localScale.magnitude / ScaleOne;
            Forward = transform.forward;
        }
    }
}
