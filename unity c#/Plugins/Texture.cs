using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Convention
{
    public static class TextureExtenion
    {
        public static Texture2D CropTexture(this Texture texture, Rect source)
        {
            RenderTexture active = RenderTexture.active;
            RenderTexture renderTexture = (RenderTexture.active = RenderTexture.GetTemporary(texture.width, texture.height, 0, RenderTextureFormat.ARGB32, RenderTextureReadWrite.Default, 8));
            bool sRGBWrite = GL.sRGBWrite;
            GL.sRGBWrite = false;
            GL.Clear(clearDepth: false, clearColor: true, new Color(1f, 1f, 1f, 0f));
            Graphics.Blit(texture, renderTexture);
            Texture2D texture2D = new Texture2D((int)source.width, (int)source.height, TextureFormat.ARGB32, mipChain: true, linear: false);
            texture2D.filterMode = FilterMode.Point;
            texture2D.ReadPixels(source, 0, 0);
            texture2D.Apply();
            GL.sRGBWrite = sRGBWrite;
            RenderTexture.active = active;
            RenderTexture.ReleaseTemporary(renderTexture);
            return texture2D;
        }
        public static Texture2D CopyTexture(this Texture texture)
        {
            return CropTexture(texture, new(0, 0, texture.width, texture.height));
        }
        public static Sprite ToSprite(this Texture2D texture)
        {
            return Sprite.Create(texture, new(0, 0, texture.width, texture.height), new(0.5f, 0.5f));
        }
    }
}
