using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Convention.VFX
{
    public class GridSpriteFullSetter : MonoAnyBehaviour
    {
        [Resources]public Transform target;
        [Resources]public MeshRenderer MyMeshRenderer;

        void Update()
        {
            MyMeshRenderer.material.SetVector("_Offset", target.position);
        }
    }
}
