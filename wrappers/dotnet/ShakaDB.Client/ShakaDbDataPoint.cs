namespace ShakaDB.Client
{
    using System;

    public class ShakaDbDataPoint
    {
        public ulong Timestamp { get; set; }

        public float Value { get; set; }

        public DateTime TimestampAsDate
        {
            get => DateTime.FromFileTime((long)Timestamp);
            set => Timestamp = (ulong)value.ToFileTime();
        }
    }
}