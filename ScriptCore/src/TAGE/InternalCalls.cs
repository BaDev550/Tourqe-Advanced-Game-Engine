using System;
using System.Runtime.CompilerServices;

namespace TAGE
{
    public static class InternalCalls
    {
        #region LOG
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativeLog(int level, string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativeLog_Vector3(int level, ref Vector3 vector);
        #endregion

        #region COMPONENTS
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetTranslation(ulong entityID, out Vector3 vector);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetTranslation(ulong entityID, ref Vector3 vector);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBodyComponent_ApplyForce(ulong entityID, ref Vector3 force);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBodyComponent_SetVelocity(ulong entityID, ref Vector3 newVelocity);
        #endregion

        #region INPUT
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyDown(KeyCode key);
        #endregion

        #region ENTITY
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(ulong entityID, Type componentType);
        #endregion
    }
}