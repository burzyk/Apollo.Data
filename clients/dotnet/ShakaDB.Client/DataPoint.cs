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

        public DataPoint(DateTimeOffset timestamp, float value)
        {
            Timestamp = (ulong) timestamp.ToUnixTimeMilliseconds();
            Value = value;
        }

        public ulong Timestamp { get; }

        public float Value { get; }

        public DateTimeOffset TimestampAsDate => DateTimeOffset.FromUnixTimeMilliseconds((long) Timestamp);
    }
}