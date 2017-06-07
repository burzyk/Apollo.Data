namespace ShakaDB.Client.Wrapper
{
    using System;
    using System.Runtime.InteropServices;

    [StructLayout(LayoutKind.Sequential)]
    internal struct SdbSession
    {
        public IntPtr Session;
    }
}