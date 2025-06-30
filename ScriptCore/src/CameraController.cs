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
        TransformComponent tc;

        void OnCreate()
        {
            tc = GetComponent<TransformComponent>();
            bool hasTransformComponent = HasComponent<TransformComponent>();
            Console.WriteLine("{0}", hasTransformComponent);
        }

        void OnUpdate(float deltaTime)
        {

        }

        void OnFixedUpdate(float deltaTime)
        {
            Vector3 velocity = Vector3.Zero;

            if (Input.IsKeyDown(KeyCode.W))
                velocity.Z = -1.0f;
            if (Input.IsKeyDown(KeyCode.S))
                velocity.Z = 1.0f;

            if (Input.IsKeyDown(KeyCode.A))
                velocity.X = -1.0f;
            if (Input.IsKeyDown(KeyCode.D))
                velocity.X = 1.0f;
            velocity *= speed;

            Vector3 pos = tc.Position;
            pos += velocity * deltaTime;
            tc.Position = pos;
        }
    }
}
