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

        //public bool V0;
        //public char V1;
        //public short V2;
        //public int V3;
        //public long V4;
        //public byte V5;
        //public UInt16 V6;
        //public UInt32 V7;
        //public UInt64 V8;
        //public double V9;
        //public Vector2 V10;
        //public Vector3 V11;
        //public Entity V12;
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

            velocity *= (speed * deltaTime);

            if (!lockMovement)
                rb.SetVelocity(velocity);
        }
    }
}
