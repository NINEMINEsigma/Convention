using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public static class MonoPlugin
{
    //������2022.3�汾��Ҳ��һ����������������˱�������ֶ��ж�
#if true
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
