using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TAGE;

namespace Sandbox
{
    public class Player : Entity
    {
        private RigidBodyComponent rb;

        public float speed = 500.0f; 
        public bool lockMovement = false;

        void OnCreate()
        {
            Logger.Log(LogLevel.Info, "Player.OnCreate");

            rb = GetComponent<RigidBodyComponent>();

            bool hasTransformComponent = HasComponent<TransformComponent>();
            Console.WriteLine("{0}", hasTransformComponent);
        }

        void OnUpdate(float deltaTime)
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

            velocity *= (speed * deltaTime);

            if (!lockMovement)
                rb.ApplyForce(velocity);
        }
    }
}
