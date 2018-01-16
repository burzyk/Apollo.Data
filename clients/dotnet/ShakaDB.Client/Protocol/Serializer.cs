namespace ShakaDB.Client.Protocol
{
    using System;
    using System.Runtime.InteropServices;

    public static class Serializer
    {
        public static T Deserialize<T>(byte[] payload)
        {
            var ptr = IntPtr.Zero;

            try
            {
                ptr = Marshal.AllocHGlobal(payload.Length);
                Marshal.Copy(payload, 0, ptr, payload.Length);
                return Marshal.PtrToStructure<T>(ptr);
            }
            finally
            {
                Marshal.FreeHGlobal(ptr);
            }
        }

        public static byte[] Serialize<T>(T payload)
        {
            var ptr = IntPtr.Zero;

            try
            {
                var size = Marshal.SizeOf(payload);
                var result = new byte[size];

                ptr = Marshal.AllocHGlobal(size);
                Marshal.StructureToPtr(payload, ptr, false);
                Marshal.Copy(ptr, result, 0, size);

                return result;
            }
            finally
            {
                Marshal.FreeHGlobal(ptr);
            }
        }
    }
}