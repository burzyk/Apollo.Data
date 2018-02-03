namespace ShakaDB.Client
{
    using System;

    public class DataPoint
    {
        public DataPoint(ulong timestamp, float value)
        {
            Timestamp = timestamp;
            Value = value;
        }

        public DataPoint(DateTime timestamp, float value)
        {
            Timestamp = (ulong) new DateTimeOffset(timestamp).ToUnixTimeMilliseconds();
            Value = value;
        }

        public ulong Timestamp { get; }

        public float Value { get; }

        public DateTime TimestampAsDate => DateTime.FromFileTime((long) Timestamp);
    }
}