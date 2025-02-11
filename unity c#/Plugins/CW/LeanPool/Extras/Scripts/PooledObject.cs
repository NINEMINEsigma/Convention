using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Lean.Pool;
using Sirenix.OdinInspector;
using UnityEngine;

public class PooledObject : MonoBehaviour, IPoolable
{
    [Tooltip("是否在生成后定时自动回收")]
    public bool autoDespawn = true;
    
    [ShowIf("autoDespawn")][Tooltip("自动回收时间")]
    public float autoDespawnTime = 1;
    
    private List<IPoolable> children;
    
    
    public void OnSpawn()
    {
        children = GetComponentsInChildren<IPoolable>().ToList();
        children.Remove(this);
        children.ForEach(child => child.OnSpawn());
        
        if (autoDespawn)
        {
            LeanPool.Despawn(gameObject, autoDespawnTime);
        }
    }

    public void OnDespawn()
    {
        children.ForEach(child => child.OnDespawn());
    }
}
