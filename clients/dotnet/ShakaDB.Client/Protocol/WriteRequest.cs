namespace ShakaDB.Client.Protocol
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Runtime.InteropServices;

    public class WriteRequest : BasePacket
    {
        public WriteRequest(uint dataSeriesId, IEnumerable<DataPoint> points)
        {
            DataSeriesId = dataSeriesId;
            Points = points.ToList();
        }

        public WriteRequest(byte[] payload)
            : base(payload)
        {
        }

        public uint DataSeriesId { get; private set; }

        public IReadOnlyList<DataPoint> Points { get; private set; }

        public override PacketType PacketType => PacketType.WriteRequest;

        protected override void Load(BinaryReader reader)
        {
            DataSeriesId = reader.ReadUInt32();
            var pointsCount = reader.ReadInt32();

            Points = Enumerable.Range(0, pointsCount)
                .Select(x => new DataPoint(reader.ReadUInt64(), reader.ReadSingle()))
                .ToList();
        }

        protected override void Save(BinaryWriter writer)
        {
            writer.Write(DataSeriesId);
            writer.Write(Points.Count);

            foreach (var point in Points)
            {
                writer.Write(point.Timestamp);
                writer.Write(point.Value);
            }
        }
    }
}