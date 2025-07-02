using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using TAGE;

namespace Sandbox
{
    public class CameraController : Entity
    {
        public float sensivity = 1.0f;
        private float _yaw = 0.0f;
        private float _pitch = 0.0f;

        private Vector2 _lastMousePosition;
        private TransformComponent _transform;

        void OnCreate()
        {
            _transform = GetComponent<TransformComponent>();

            Vector3 initialRotation = _transform.Rotation;
            _yaw = initialRotation.Y;
            _pitch = initialRotation.X;
        }

        void OnUpdate(float deltaTime)
        {
            HandleMouseLook(deltaTime);
        }

        private void HandleMouseLook(float deltaTime)
        {
            Vector2 currentMousePos = Input.GetMousePos();
            Vector2 mouseDelta = currentMousePos - _lastMousePosition;
            _lastMousePosition = currentMousePos;

            _yaw += mouseDelta.X * sensivity * deltaTime;
            _pitch -= mouseDelta.Y * sensivity * deltaTime;

            _transform.Rotation = new Vector3(-_pitch, _yaw, 0.0f);
        }
    }
}
