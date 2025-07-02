using System;
using System.IO;

namespace TAGE
{
    public struct Vector3
    {
        public float X, Y, Z;
        public static Vector3 Zero => new Vector3(0.0f);

        public Vector3(float scalar)
        {
            X = scalar;
            Y = scalar;
            Z = scalar;
        }

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        public Vector2 XY
        {
            get => new Vector2(X, Y);
            set
            {
                X = value.X;
                Y = value.Y;
            }
        }

        public float Length()
        {
            float result = (float)Math.Sqrt(X * X + Y * Y + Z * Z);
            return result;
        }

        public static Vector3 Normalize(Vector3 v)
        {
            float length = (float)Math.Sqrt(v.X * v.X + v.Y * v.Y + v.Z * v.Z);

            if (length > 0f)
            {
                v.X /= length;
                v.Y /= length;
                v.Z /= length;
            }

            return v;
        }

        public static Vector3 operator *(Vector3 vector, float scaler)
        {
            return new Vector3(vector.X * scaler, vector.Y * scaler, vector.Z * scaler);
        }
        public static Vector3 operator +(Vector3 a, Vector3 b)
        {
            return new Vector3(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
        }
        public static Vector3 operator -(Vector3 a, Vector3 b)
        {
            return new Vector3(a.X - b.X, a.Y - b.Y, a.Z - b.Z);
        }
    };

    public struct Vector2
    {
        public float X, Y;
        public static Vector2 Zero => new Vector2(0.0f);

        public Vector2(float scalar)
        {
            X = scalar;
            Y = scalar;
        }

        public Vector2(float x, float y)
        {
            X = x;
            Y = y;
        }

        public static Vector2 operator *(Vector2 vector, float scaler)
        {
            return new Vector2(vector.X * scaler, vector.Y * scaler);
        }
        public static Vector2 operator +(Vector2 a, Vector2 b)
        {
            return new Vector2(a.X + b.X, a.Y + b.Y);
        }
        public static Vector2 operator -(Vector2 a, Vector2 b)
        {
            return new Vector2(a.X - b.X, a.Y - b.Y);
        }
    };
}
