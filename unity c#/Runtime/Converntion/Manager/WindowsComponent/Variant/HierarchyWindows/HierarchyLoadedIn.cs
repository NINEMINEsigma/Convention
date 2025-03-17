using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Convention
{
    public interface ILoadedInHierarchy { }
    public interface IOnlyLoadedInHierarchy { }
    public class HierarchyLoadedIn : MonoAnyBehaviour
    {
        private void Update()
        {
            if (!RegisterBaseWrapperExtension.Registers.ContainsKey(typeof(WindowsUI.Variant.HierarchyWindow)))
                return;
            var onlys = GetComponents<IOnlyLoadedInHierarchy>();
            try
            {
                if (onlys != null && onlys.Length != 0)
                {
                    WindowsUI.Variant.HierarchyWindow.instance.CreateRootItemEntryWithBinders(onlys[0])[0].ref_value.GetComponent<WindowsUI.Variant.HierarchyItem>().title = $"{name}";
                }
                else
                {
                    var goItem = WindowsUI.Variant.HierarchyWindow.instance.CreateRootItemEntryWithGameObject(gameObject);
                    goItem.ref_value.GetComponent<WindowsUI.Variant.HierarchyItem>().title = $"{name}";
                    foreach (var item in GetComponents<ILoadedInHierarchy>())
                    {
                        goItem.ref_value.GetComponent<WindowsUI.Variant.HierarchyItem>().CreateSubPropertyItemWithBinders(item)[0]
                            .ref_value
                            .GetComponent<WindowsUI.Variant.HierarchyItem>()
                            .title = $"{name}-{item.GetType()}";
                    }
                }
            }
            catch (Exception) { }
            finally
            {
                GameObject.DestroyImmediate(this);
            }
        }
    }
}
