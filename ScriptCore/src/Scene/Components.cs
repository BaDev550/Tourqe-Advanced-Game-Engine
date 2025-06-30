using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TAGE
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }

    public class TransformComponent : Component
    {
        public Vector3 Position
        {
            get
            {
                InternalCalls.TransformComponent_GetTranslation(Entity.ID, out Vector3 position);
                return position;
            }

            set
            {
                InternalCalls.TransformComponent_SetTranslation(Entity.ID, ref value);
            }
        }
    }

    public class RigidBodyComponent : Component
    {
        public void ApplyForce(Vector3 force)
        {
            InternalCalls.RigidBodyComponent_ApplyForce(Entity.ID, ref force);
        }

        public void SetVelocity(Vector3 velocity)
        {
            InternalCalls.RigidBodyComponent_SetVelocity(Entity.ID, ref velocity);
        }
    }
}
