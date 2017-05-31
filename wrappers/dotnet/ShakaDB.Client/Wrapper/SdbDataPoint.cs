namespace ShakaDB.Client.Wrapper
{
    using System.Runtime.InteropServices;


    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    internal struct SdbDataPoint
    {
        public ulong Time;

        public float Value;
    }
}