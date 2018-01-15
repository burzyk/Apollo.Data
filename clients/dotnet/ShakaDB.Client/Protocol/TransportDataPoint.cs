namespace ShakaDB.Client.Protocol
{
    using System.Runtime.InteropServices;

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct TransportDataPoint
    {
        public ulong Time;

        public float Value;
    }
}