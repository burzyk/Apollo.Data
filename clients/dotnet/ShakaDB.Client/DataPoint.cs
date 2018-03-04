namespace ShakaDB.Client
{
    using System;
    using System.Linq;
    using System.Text;

    public class DataPoint : IEquatable<DataPoint>
    {
        public DataPoint(ulong timestamp, byte[] value)
        {
            Timestamp = timestamp;
            Value = value;
        }

        public DataPoint(ulong timestamp, float value)
            : this(timestamp, BitConverter.GetBytes(value))
        {
        }

        public DataPoint(DateTimeOffset timestamp, byte[] value)
            : this((ulong) timestamp.ToUnixTimeMilliseconds(), value)
        {
        }

        public DataPoint(DateTimeOffset timestamp, int value)
            : this(timestamp, BitConverter.GetBytes(value))
        {
        }

        public DataPoint(DateTimeOffset timestamp, long value)
            : this(timestamp, BitConverter.GetBytes(value))
        {
        }

        public DataPoint(DateTimeOffset timestamp, float value)
            : this(timestamp, BitConverter.GetBytes(value))
        {
        }

        public DataPoint(DateTimeOffset timestamp, double value)
            : this(timestamp, BitConverter.GetBytes(value))
        {
        }

        public DataPoint(DateTimeOffset timestamp, string value)
            : this(timestamp, Encoding.ASCII.GetBytes(value))
        {
        }

        public ulong Timestamp { get; }

        public byte[] Value { get; }

        public DateTimeOffset TimestampAsDate => DateTimeOffset.FromUnixTimeMilliseconds((long) Timestamp);

        public int ValueAsInt32 => BitConverter.ToInt32(Value, 0);

        public long ValueAsInt64 => BitConverter.ToInt64(Value, 0);

        public float ValueAsFloat => BitConverter.ToSingle(Value, 0);

        public double ValueAsDouble => BitConverter.ToDouble(Value, 0);

        public string ValueAsString => Encoding.ASCII.GetString(Value.TakeWhile(x => x != 0).ToArray());

        public bool Equals(DataPoint other)
        {
            return Timestamp == other?.Timestamp && Value.SequenceEqual(other.Value ?? new byte[] { });
        }
    }
}