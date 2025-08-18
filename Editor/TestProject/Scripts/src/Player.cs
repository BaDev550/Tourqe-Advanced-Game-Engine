using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TAGE;

namespace Sandbox
{
    public class Player : Entity
    {
        private RigidBodyComponent rb;
        private TransformComponent transform;

        public float speed = 500.0f;
        public float sens;
        public bool lockMovement = false;

        private float xRotation = 0;
        private float yRotation = 0;

        void OnCreate()
        {
            Logger.Log(LogLevel.Info, "Player.OnCreate");

            rb = GetComponent<RigidBodyComponent>();
            transform = GetComponent<TransformComponent>();

            bool hasTransformComponent = HasComponent<TransformComponent>();
            Console.WriteLine("{0}", hasTransformComponent);
        }

        void OnUpdate(float deltaTime)
        {
            Vector3 velocity = Vector3.Zero;
            Vector2 mouseDelta = Input.GetMouseDelta() * sens;

            yRotation = mouseDelta.X;
            xRotation = TMath.Clamp(-mouseDelta.Y, -90f, 90f);
            //transform.Rotation = new Vector3(xRotation, yRotation, 0);

            float yawRadians = yRotation * (float)(Math.PI / 180f);
            Vector3 forward = new Vector3((float)Math.Sin(yawRadians), 0, (float)Math.Cos(yawRadians));
            Vector3 right = new Vector3(forward.Z, 0, -forward.X);

            if (Input.IsKeyDown(KeyCode.W)) velocity -= forward;
            if (Input.IsKeyDown(KeyCode.S)) velocity += forward;

            if (Input.IsKeyDown(KeyCode.A)) velocity -= right;
            if (Input.IsKeyDown(KeyCode.D)) velocity += right;

            if (velocity.Length() > 0)
                velocity = Vector3.Normalize(velocity);

            velocity *= (speed * deltaTime);

            if (!lockMovement)
                rb.ApplyForce(velocity);
        }
    }
}
