namespace ShakaDB.Client.Wrapper
{
    using System;
    using System.Runtime.InteropServices;

    [StructLayout(LayoutKind.Sequential)]
    internal struct SdbDataPointsIterator
    {
        public IntPtr RawPoints;

        public int PointsCount;

        public IntPtr Iterator;
        
        public IntPtr Session;

        public SdbDataPoint ReadAt(int offset)
        {
            if (offset >= PointsCount)
            {
                throw new ArgumentOutOfRangeException(nameof(offset));
            }

            return Marshal.PtrToStructure<SdbDataPoint>(RawPoints + Marshal.SizeOf<SdbDataPoint>() * offset);
        }
    }
}