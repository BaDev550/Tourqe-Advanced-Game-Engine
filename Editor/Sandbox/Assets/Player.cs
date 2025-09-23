using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TAGE;

namespace Sandbox
{
    class Player : Entity
    {
        public float speed = 50.0f;
        private RigidBodyComponent rb;

        public void OnCreate()
        {
            rb = GetComponent<RigidBodyComponent>();
        }

        public void OnUpdate(float deltaTime)
        {
            Vector3 velocity = new Vector3();
            if (Input.IsKeyDown(KeyCode.W)) velocity.X -= (1.0f * speed) * deltaTime;
            if (Input.IsKeyDown(KeyCode.S)) velocity.X += (1.0f * speed) * deltaTime;
            if (Input.IsKeyDown(KeyCode.A)) velocity.Z -= (1.0f * speed) * deltaTime;
            if (Input.IsKeyDown(KeyCode.D)) velocity.Z += (1.0f * speed) * deltaTime;

            rb.ApplyForce(velocity);
        }
    }
}
