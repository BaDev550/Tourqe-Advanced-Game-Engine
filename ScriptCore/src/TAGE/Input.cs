using System;

namespace TAGE
{
    public enum CursorMode
    {
        Normal = 0,
        Hidden = 1,
        Locked = 2
    }

    public class Input
    {
        public static bool IsKeyDown(KeyCode keyCode)
        {
            return InternalCalls.Input_IsKeyDown(keyCode);
        }

        public static Vector2 GetMousePos()
        {
            InternalCalls.Input_GetMousePos(out Vector2 position);
            return position;
        }

        public static Vector2 GetMouseDelta()
        {
            InternalCalls.Input_GetMousePos(out Vector2 delta);
            return delta;
        }
    }
}
