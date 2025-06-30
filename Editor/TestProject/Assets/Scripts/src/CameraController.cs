using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TAGE;

namespace Sandbox
{
    public class CameraController : Entity
    {
        float speed = 5.0f;
        public float sensivity = 1.0f;

        void OnUpdate(float deltaTime)
        {
            Vector3 velocity = Vector3.Zero;

            if (Input.IsKeyDown(KeyCode.Up))
                velocity.Z = -1.0f;
            if (Input.IsKeyDown(KeyCode.Down))
                velocity.Z = 1.0f;

            if (Input.IsKeyDown(KeyCode.Right))
                velocity.X = -1.0f;
            if (Input.IsKeyDown(KeyCode.Left))
                velocity.X = 1.0f;
            velocity *= speed;

            Vector3 pos = Translation;
            pos += velocity * deltaTime;
            Translation = pos;
        }
    }
}
