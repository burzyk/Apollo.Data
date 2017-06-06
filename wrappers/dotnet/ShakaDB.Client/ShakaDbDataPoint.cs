namespace ShakaDB.Client
{
    using System;

    public class ShakaDbDataPoint
    {
        public ShakaDbDataPoint(ulong timestamp, float value)
        {
            Timestamp = timestamp;
            Value = value;
        }

        public ShakaDbDataPoint(DateTime timestamp, float value)
        {
            Timestamp = (ulong) timestamp.ToFileTime();
            Value = value;
        }

        public ulong Timestamp { get; }

        public float Value { get; }

        public DateTime TimestampAsDate => DateTime.FromFileTime((long) Timestamp);
    }
}