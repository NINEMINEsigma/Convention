using System;
using System.Collections;
using System.Collections.Generic;
using Convention.WindowsUI;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using UnityEditor;
using UnityEngine.Profiling;
using static UnityEngine.UI.GridLayoutGroup;

namespace Convention
{
    public static class UnityExtension
    {
        public static void InitExtensionEnv()
        {
            AsyncOperationExtension.InitExtensionEnv();
            RectTransformExtension.InitExtensionEnv();
        }
    }

    public static class AsyncOperationExtension
    {
        public static void InitExtensionEnv()
        {
            CompletedHelper.InitExtensionEnv();
        }
        public static void MarkCompleted(this AsyncOperation operation, [In] Action action)
        {
            operation.completed += new CompletedHelper(action).InternalCompleted;
        }

        private class CompletedHelper
        {
            static CompletedHelper() => helpers = new();
            public static void InitExtensionEnv() => helpers.Clear();
            private static readonly List<CompletedHelper> helpers = new();

            readonly Action action;

            public CompletedHelper([In] Action action)
            {
                helpers.Add(this);
                this.action = action;
            }
            ~CompletedHelper()
            {
                helpers.Remove(this);
            }

            public void InternalCompleted(AsyncOperation obj)
            {
                if (obj.progress < 0.99f) return;
                action.Invoke();
                helpers.Remove(this);
            }

        }
    }

    public static partial class RectTransformExtension
    {
        private static bool IsDisableAdjustSizeToContainsChilds2DeferUpdates = false;


        public static void InitExtensionEnv()
        {
            IsDisableAdjustSizeToContainsChilds2DeferUpdates = false;
        }

        public class AdjustSizeIgnore : MonoAnyBehaviour { }
#if UNITY_EDITOR
        [UnityEditor.MenuItem("Convention/DisableAdjustSize", priority = 100000)]
#endif
        public static void DisableAdjustSizeToContainsChilds2DeferUpdates()
        {
            IsDisableAdjustSizeToContainsChilds2DeferUpdates = true;
        }
#if UNITY_EDITOR
        [UnityEditor.MenuItem("Convention/EnableAdjustSize", priority = 100000)]
#endif
        public static void AppleAndEnableAdjustSizeToContainsChilds()
        {
            IsDisableAdjustSizeToContainsChilds2DeferUpdates = false;
        }
        public static void AdjustSizeToContainsChilds([In] RectTransform rectTransform, Vector2 min, Vector2 max, RectTransform.Axis? axis)
        {
            if (IsDisableAdjustSizeToContainsChilds2DeferUpdates)
                return;
            LayoutRebuilder.ForceRebuildLayoutImmediate(rectTransform);

            bool stats = false;

            List<RectTransform> currentList = new(), nextList = new();
            var corners = new Vector3[4];
            foreach (RectTransform item in rectTransform)
            {
                currentList.Add(item);
            }
            do
            {
                currentList.AddRange(nextList);
                nextList.Clear();
                foreach (RectTransform childTransform in currentList)
                {
                    if (childTransform.gameObject.activeInHierarchy == false)
                        continue;
                    if (childTransform.name.ToLower().Contains("<ignore rect>"))
                        continue;
                    if (childTransform.name.ToLower().Contains($"<ignore {nameof(AdjustSizeToContainsChilds)}>"))
                        continue;
                    if (childTransform.GetComponents<AdjustSizeIgnore>().Length != 0)
                        continue;
                    stats = true;
                    foreach (RectTransform item in childTransform)
                    {
                        nextList.Add(item);
                    }
                    childTransform.GetWorldCorners(corners);
                    foreach (var corner in corners)
                    {
                        Vector2 localCorner = rectTransform.InverseTransformPoint(corner);
                        if (float.IsNaN(localCorner.x) || float.IsNaN(localCorner.y))
                            break;
                        min.x = Mathf.Min(min.x, localCorner.x);
                        min.y = Mathf.Min(min.y, localCorner.y);
                        max.x = Mathf.Max(max.x, localCorner.x);
                        max.y = Mathf.Max(max.y, localCorner.y);
                    }
                }
                currentList.Clear();
            } while (nextList.Count > 0);
            if (stats)
            {
                if ((axis.HasValue && axis.Value == RectTransform.Axis.Vertical) ||
                    (rectTransform.anchorMin.x == 0 && rectTransform.anchorMax.x == 1 && rectTransform.anchorMin.y == rectTransform.anchorMax.y))
                {
                    rectTransform.SetSizeWithCurrentAnchors(RectTransform.Axis.Vertical, max.y - min.y);
                }
                else if ((axis.HasValue && axis.Value == RectTransform.Axis.Horizontal) ||
                    (rectTransform.anchorMin.y == 0 && rectTransform.anchorMax.y == 1 && rectTransform.anchorMin.x == rectTransform.anchorMax.x))
                {
                    rectTransform.SetSizeWithCurrentAnchors(RectTransform.Axis.Horizontal, max.x - min.x);
                }
                else
                {
                    rectTransform.SetSizeWithCurrentAnchors(RectTransform.Axis.Horizontal, max.x - min.x);
                    rectTransform.SetSizeWithCurrentAnchors(RectTransform.Axis.Vertical, max.y - min.y);
                }
            }
        }
        public static void AdjustSizeToContainsChilds([In] RectTransform rectTransform, RectTransform.Axis axis)
        {
            if (IsDisableAdjustSizeToContainsChilds2DeferUpdates)
                return;
            Vector2 min = new Vector2(float.MaxValue, float.MaxValue);
            Vector2 max = new Vector2(float.MinValue, float.MinValue);

            AdjustSizeToContainsChilds(rectTransform, min, max, axis);
        }
        public static void AdjustSizeToContainsChilds([In] RectTransform rectTransform)
        {
            if (IsDisableAdjustSizeToContainsChilds2DeferUpdates)
                return;
            Vector2 min = new Vector2(float.MaxValue, float.MaxValue);
            Vector2 max = new Vector2(float.MinValue, float.MinValue);

            AdjustSizeToContainsChilds(rectTransform, min, max, null);
        }

        internal static void SetParentAndResizeWithoutNotifyBaseWindowPlane([In] RectTransform parent, [In] RectTransform child, Rect rect, bool isAdjustSizeToContainsChilds)
        {
            child.SetParent(parent);
            child.SetInsetAndSizeFromParentEdge(RectTransform.Edge.Left, rect.x, rect.width);
            child.SetInsetAndSizeFromParentEdge(RectTransform.Edge.Top, rect.y, rect.height);
            if (isAdjustSizeToContainsChilds)
                AdjustSizeToContainsChilds(parent);
        }
        internal static void SetParentAndResizeWithoutNotifyBaseWindowPlane(RectTransform parent, RectTransform child, bool isAdjustSizeToContainsChilds)
        {
            child.SetParent(parent);
            if (isAdjustSizeToContainsChilds)
                AdjustSizeToContainsChilds(parent);
        }
        public static void SetParentAndResize(RectTransform parent, RectTransform child, Rect rect, bool isAdjustSizeToContainsChilds)
        {
            if (parent.GetComponents<BaseWindowPlane>().Length != 0)
            {
                parent.GetComponents<BaseWindowPlane>()[0].AddChild(child, rect, isAdjustSizeToContainsChilds);
            }
            else
            {
                SetParentAndResizeWithoutNotifyBaseWindowPlane(parent, child, rect, isAdjustSizeToContainsChilds);
            }
        }
        public static void SetParentAndResize(RectTransform parent, RectTransform child, bool isAdjustSizeToContainsChilds)
        {
            if (parent.GetComponents<BaseWindowPlane>().Length != 0)
            {
                parent.GetComponents<BaseWindowPlane>()[0].AddChild(child, isAdjustSizeToContainsChilds);
            }
            else
            {
                SetParentAndResizeWithoutNotifyBaseWindowPlane(parent, child, isAdjustSizeToContainsChilds);
            }
        }

        public static bool IsVisible([In]RectTransform rectTransform, [In,Opt] Camera camera=null)
        {
            if (camera == null)
                camera = Camera.main;
            Transform camTransform = camera.transform;
            Vector3[] corners = new Vector3[4];
            rectTransform.GetWorldCorners(corners);
            foreach (var worldPos in corners)
            {
                Vector2 viewPos = camera.WorldToViewportPoint(worldPos);
                Vector3 dir = (worldPos - camTransform.position).normalized;
                float dot = Vector3.Dot(camTransform.forward, dir);

                if (dot <= 0 || viewPos.x < 0 || viewPos.x > 1 || viewPos.y < 0 || viewPos.y > 1)
                    return false;
            }
            return true;
        }
    }

    public static partial class SkyExtension
    {
        public static Material GetSky()
        {
            return RenderSettings.skybox;
        }

        public static void Load([In][Opt,When("If you sure")]Material skybox)
        {
            RenderSettings.skybox = skybox;
        }

        public static void Rotation(float angle)
        {
            RenderSettings.skybox.SetFloat("_Rotation", angle);
        }
    }

    public static partial class SceneExtension
    {
        public static void Load(string name)
        {
            SceneManager.LoadScene(name, LoadSceneMode.Additive);
        }
        public static void Load(string name, out AsyncOperation async)
        {
            async = SceneManager.LoadSceneAsync(name, LoadSceneMode.Additive);
        }
        public static void Unload(string name)
        {
            SceneManager.UnloadSceneAsync(name);
        }
        public static Scene GetScene(string name)
        {
            return SceneManager.GetSceneByName(name);
        }
    }

    public static class GameObjectExtension
    {
        /// <summary>
        /// 递归设置GameObject及其所有子物体的Layer
        /// </summary>
        public static void SetLayerRecursively(GameObject gameObject, int layer)
        {
            gameObject.layer = layer;
            foreach (Transform t in gameObject.transform)
            {
                SetLayerRecursively(t.gameObject, layer);
            }
        }

        /// <summary>
        /// 递归设置GameObject及其所有子物体的Tag
        /// </summary>
        public static void SetTagRecursively(GameObject gameObject, string tag)
        {
            gameObject.tag = tag;
            foreach (Transform t in gameObject.transform)
            {
                SetTagRecursively(t.gameObject, tag);
            }
        }

        /// <summary>
        /// 递归启用/禁用所有Collider组件
        /// </summary>
        public static void SetCollisionRecursively(GameObject gameObject, bool enabled)
        {
            var colliders = gameObject.GetComponentsInChildren<Collider>();
            foreach (var collider in colliders)
            {
                collider.enabled = enabled;
            }
        }

        /// <summary>
        /// 递归启用/禁用所有Renderer组件
        /// </summary>
        public static void SetVisualRecursively(GameObject gameObject, bool enabled)
        {
            var renderers = gameObject.GetComponentsInChildren<Renderer>();
            foreach (var renderer in renderers)
            {
                renderer.enabled = enabled;
            }
        }

        /// <summary>
        /// 获取指定Tag的所有子组件
        /// </summary>
        public static T[] GetComponentsInChildrenWithTag<T>(GameObject gameObject, string tag) where T : Component
        {
            List<T> results = new List<T>();
            
            if (gameObject.CompareTag(tag))
            {
                var component = gameObject.GetComponent<T>();
                if (component != null)
                    results.Add(component);
            }

            foreach (Transform t in gameObject.transform)
            {
                results.AddRange(GetComponentsInChildrenWithTag<T>(t.gameObject, tag));
            }

            return results.ToArray();
        }

        /// <summary>
        /// 在父物体中查找组件
        /// </summary>
        public static T GetComponentInParents<T>(GameObject gameObject) where T : Component
        {
            for (Transform t = gameObject.transform; t != null; t = t.parent)
            {
                T result = t.GetComponent<T>();
                if (result != null)
                    return result;
            }
            return null;
        }

        /// <summary>
        /// 获取或添加组件
        /// </summary>
        public static T GetOrAddComponent<T>(GameObject gameObject) where T : Component
        {
            T component = gameObject.GetComponent<T>();
            return component ?? gameObject.AddComponent<T>();
        }
    }

    public static class TransformExtension
    {
        /// <summary>
        /// 获取所有子物体
        /// </summary>
        public static List<Transform> GetAllChildren(this Transform transform)
        {
            List<Transform> children = new List<Transform>();
            foreach (Transform child in transform)
            {
                children.Add(child);
                children.AddRange(child.GetAllChildren());
            }
            return children;
        }

        /// <summary>
        /// 销毁所有子物体
        /// </summary>
        public static void DestroyAllChildren(this Transform transform)
        {
            for (int i = transform.childCount - 1; i >= 0; i--)
            {
                UnityEngine.Object.Destroy(transform.GetChild(i).gameObject);
            }
        }

        /// <summary>
        /// 设置父物体并保持世界坐标
        /// </summary>
        public static void SetParentKeepWorldPosition(this Transform transform, Transform parent)
        {
            Vector3 worldPos = transform.position;
            Quaternion worldRot = transform.rotation;
            transform.SetParent(parent);
            transform.position = worldPos;
            transform.rotation = worldRot;
        }
    }

    public static class CoroutineExtension
    {
        /// <summary>
        /// 延迟执行
        /// </summary>
        public static IEnumerator Delay(float delay, Action action)
        {
            yield return new WaitForSeconds(delay);
            action?.Invoke();
        }

        /// <summary>
        /// 延迟执行并返回结果
        /// </summary>
        public static IEnumerator Delay<T>(float delay, Func<T> action, Action<T> callback)
        {
            yield return new WaitForSeconds(delay);
            callback?.Invoke(action());
        }

        /// <summary>
        /// 等待直到条件满足
        /// </summary>
        public static IEnumerator WaitUntil(Func<bool> condition, Action onComplete = null)
        {
            yield return new WaitUntil(condition);
            onComplete?.Invoke();
        }

        /// <summary>
        /// 等待直到条件满足，带超时
        /// </summary>
        public static IEnumerator WaitUntil(Func<bool> condition, float timeout, Action onComplete = null, Action onTimeout = null)
        {
            float elapsedTime = 0;
            while (!condition() && elapsedTime < timeout)
            {
                elapsedTime += Time.deltaTime;
                yield return null;
            }

            if (elapsedTime >= timeout)
            {
                onTimeout?.Invoke();
            }
            else
            {
                onComplete?.Invoke();
            }
        }

        /// <summary>
        /// 执行动画曲线
        /// </summary>
        public static IEnumerator Animate(float duration, AnimationCurve curve, Action<float> onUpdate)
        {
            float elapsedTime = 0;
            while (elapsedTime < duration)
            {
                elapsedTime += Time.deltaTime;
                float normalizedTime = elapsedTime / duration;
                float evaluatedValue = curve.Evaluate(normalizedTime);
                onUpdate?.Invoke(evaluatedValue);
                yield return null;
            }
            onUpdate?.Invoke(curve.Evaluate(1));
        }

        /// <summary>
        /// 执行线性插值
        /// </summary>
        public static IEnumerator Lerp<T>(T start, T end, float duration, Action<T> onUpdate, Func<T, T, float, T> lerpFunction)
        {
            float elapsedTime = 0;
            while (elapsedTime < duration)
            {
                elapsedTime += Time.deltaTime;
                float normalizedTime = elapsedTime / duration;
                T current = lerpFunction(start, end, normalizedTime);
                onUpdate?.Invoke(current);
                yield return null;
            }
            onUpdate?.Invoke(end);
        }

        /// <summary>
        /// 执行Vector3插值
        /// </summary>
        public static IEnumerator LerpVector3(Vector3 start, Vector3 end, float duration, Action<Vector3> onUpdate)
        {
            yield return Lerp(start, end, duration, onUpdate, Vector3.Lerp);
        }

        /// <summary>
        /// 执行Quaternion插值
        /// </summary>
        public static IEnumerator LerpQuaternion(Quaternion start, Quaternion end, float duration, Action<Quaternion> onUpdate)
        {
            yield return Lerp(start, end, duration, onUpdate, Quaternion.Lerp);
        }

        /// <summary>
        /// 执行float插值
        /// </summary>
        public static IEnumerator LerpFloat(float start, float end, float duration, Action<float> onUpdate)
        {
            yield return Lerp(start, end, duration, onUpdate, Mathf.Lerp);
        }
    }

    public static class ScriptingDefineUtility
    {
#if UNITY_EDITOR
        public static void Add(string define, BuildTargetGroup target, bool log = false)
        {
            string definesString = PlayerSettings.GetScriptingDefineSymbolsForGroup(target);
            if (definesString.Contains(define)) return;
            string[] allDefines = definesString.Split(';');
            ArrayUtility.Add(ref allDefines, define);
            definesString = string.Join(";", allDefines);
            PlayerSettings.SetScriptingDefineSymbolsForGroup(target, definesString);
            Debug.Log("Added \"" + define + "\" from " + EditorUserBuildSettings.selectedBuildTargetGroup + " Scripting define in Player Settings");
        }

        public static void Remove(string define, BuildTargetGroup target, bool log = false)
        {
            string definesString = PlayerSettings.GetScriptingDefineSymbolsForGroup(target);
            if (!definesString.Contains(define)) return;
            string[] allDefines = definesString.Split(';');
            ArrayUtility.Remove(ref allDefines, define);
            definesString = string.Join(";", allDefines);
            PlayerSettings.SetScriptingDefineSymbolsForGroup(target, definesString);
            Debug.Log("Removed \"" + define + "\" from " + EditorUserBuildSettings.selectedBuildTargetGroup + " Scripting define in Player Settings");
        }
#endif
    }
}
