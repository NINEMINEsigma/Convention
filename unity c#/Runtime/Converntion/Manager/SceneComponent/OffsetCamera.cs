using UnityEngine;

namespace Convention
{
    public class OffsetCamera : MonoAnyBehaviour
    {
        [Tooltip("The camera which this camera will follow with an offset")]
        [Resources, SerializeField] private Camera m_DataCamera;
        [Resources, SerializeField] private Camera m_FollowCarmera;

        [Content, OnlyPlayMode, SerializeField] private Vector3 m_Offset;

        private void Start()
        {
            if (m_DataCamera == null)
                m_DataCamera = Camera.main;
            if (m_FollowCarmera == null)
                m_FollowCarmera = GetComponent<Camera>();
        }

        public void ToggleOffset()
        {
            m_Offset = -m_Offset;
        }

        public void SetOffset(Vector3 offset)
        {
            m_Offset = offset;
        }

        public Vector3 GetOffset()
        {
            return m_Offset;
        }

        public void UpdateWithOffset()
        {
            Transform mainCamTransform = m_DataCamera.transform;

            transform.SetPositionAndRotation(mainCamTransform.position + m_Offset, mainCamTransform.rotation);

            m_FollowCarmera.fieldOfView = m_DataCamera.fieldOfView;
        }
    }
}
