using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using Convention.WindowsUI;
using UnityEngine;

namespace Convention.SO
{
    [CreateAssetMenu(fileName = "new WindowsConfig", menuName = "Convention/WindowsConfig", order = 200)]
    public class Windows : ScriptableObject
    {
        public static string GlobalWindowsConfig = "WindowConfig";
        public static Windows GlobalInstance => Resources.Load<Windows>(GlobalWindowsConfig);

        public static void InitExtensionEnv()
        {
            default_exist_names = GetDefaultNames();
#if CONVENTION_DISABLE_WINDOWSO_GLOBAL_INIT
            GlobalWindowsConfig = "WindowConfig";
#endif
        }
        public static string[] GetDefaultNames()
        {
            List<string> names = new();
            foreach (var item in Assembly.GetAssembly(typeof(Windows)).GetTypes())
            {
                if (item.IsSubclassOf(typeof(WindowsComponent)) ||
                    (item.IsInterface == false && item.GetInterface(nameof(IWindowUIModule)) != null)
                    )
                {
                    names.Add(item.Name);
                }
            }
            names.Add(nameof(WindowManager));
            names.Remove(nameof(WindowUIModule));
            return names.ToArray();
        }
        private static string[] default_exist_names = GetDefaultNames();

        private void OnEnable()
        {
            Reset();
        }

        public override void Reset()
        {
            base.Reset();
            foreach (string name in default_exist_names)
            {
                var resourcesArray = Resources.LoadAll(name);
                foreach (var item in resourcesArray)
                {
                    if (item is not GameObject)
                        continue;
                    if((item as GameObject).GetComponents<IAnyClass>().Length == 0)
                        continue;
                    this.uobjects[name] = item;
                    break;
                }
            }
        }

        [return: When("Datas's keys contains [In]name"), ReturnMayNull]
        public WindowsComponent[] GetWindowsComponents([In] string name)
        {
            if (this.uobjects.TryGetValue(name, out var uobj))
            {
                var go = (uobj as GameObject);
                return go.GetComponents<WindowsComponent>();
            }
            else return null;
        }
        [return: When("Datas's keys contains [In]name"), IsInstantiated(false), ReturnMayNull]
        public WindowsComponent GetWindowsComponent([In] string name)
        {
            var wc = GetWindowsComponents(name);
            if (wc.Length == 0)
                return null;
            return wc[0];
        }
        [return: When("Datas's keys contains [In]name and instance is T"), IsInstantiated(false)]
        public T GetWindowsComponent<T>([In] string name) where T : WindowsComponent
        {
            return GetWindowsComponents(name).FirstOrDefault(P => (P as T) != null) as T;
        }

        [return: When("Datas's keys contains [In]name"), ReturnMayNull]
        public IWindowUIModule[] GetWindowsUIs([In] string name)
        {
            if (this.uobjects.TryGetValue(name, out var value))
                return (value as GameObject).GetComponents<IWindowUIModule>();
            return null;
        }
        [return: When("Datas's keys contains [In]name"), IsInstantiated(false), ReturnMayNull]
        public IWindowUIModule GetWindowsUI([In] string name)
        {
            var wm = GetWindowsUIs(name);
            if (wm.Length == 0)
                return null;
            return wm[0];
        }
        [return: When("Datas's keys contains [In]name and instance is T"), IsInstantiated(false)]
        public T GetWindowsUI<T>([In] string name) where T : class, IWindowUIModule
        {
            return GetWindowsUIs(name).FirstOrDefault(P => (P as T) != null) as T;
        }
    }
}
