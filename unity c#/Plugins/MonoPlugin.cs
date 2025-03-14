using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public static class MonoPlugin
{
    //即便是2022.3版本中也不一定有这两个方法因此必须进行手动判断
#if false
    public static int GetComponentCount(this GameObject go)
    {
        return go.GetComponents<Component>().Length;
    }

    public static Component GetComponentAtIndex(this GameObject go, int index)
    {
        return go.GetComponents<Component>()[index];
    }
#endif
}
