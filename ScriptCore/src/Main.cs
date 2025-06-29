using System;
using System.Runtime.CompilerServices;

namespace TAGE
{
    public struct Vector3
    {
        public float X, Y, Z;

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }
    };

    public enum LogLevel { Info, Warn, Error }
    public static class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativeLog(int level, string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativeLog_Vector3(int level, ref Vector3 vector);
    }

    public static class Logger
    {
        internal static void Log(LogLevel level, string text) { InternalCalls.NativeLog((int)level, text); }
        internal static void Log(LogLevel level, Vector3 vector) { InternalCalls.NativeLog_Vector3((int)level, ref vector); }
    }

    public class Entity
    {
        public Entity()
        {
            Logger.Log(LogLevel.Info, "Main Constructor!");
            Vector3 Pos = new Vector3(5, 1, 2);
            Logger.Log(LogLevel.Error, Pos);
            Logger.Log(LogLevel.Warn, "Test");
        }

        public void PrintMessage()
        {
            Logger.Log(LogLevel.Warn, "Hello C# World!");
        }

        public void PrintInt(int value)
        {
            Logger.Log(LogLevel.Warn, $"C# says: {value}");
        }

        public void Print(string message)
        {
            Logger.Log(LogLevel.Warn, $"C# says: {message}");
        }
    }
}