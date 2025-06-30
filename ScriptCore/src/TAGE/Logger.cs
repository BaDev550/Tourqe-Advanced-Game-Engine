using System;

namespace TAGE
{
    public enum LogLevel { Info, Warn, Error }
    public static class Logger
    {
        public static void Log(LogLevel level, string text) { InternalCalls.NativeLog((int)level, text); }
        public static void Log(LogLevel level, Vector3 vector) { InternalCalls.NativeLog_Vector3((int)level, ref vector); }
    }
}
