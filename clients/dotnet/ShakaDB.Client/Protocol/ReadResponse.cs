namespace ShakaDB.Client.Protocol
{
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;

    public class ReadResponse : BasePacket
    {
        public ReadResponse(IReadOnlyList<DataPoint> points)
        {
            Points = points;
        }

        public ReadResponse(byte[] payload)
            : base(payload)
        {
        }

        public IReadOnlyList<DataPoint> Points { get; private set; }

        public override PacketType PacketType => PacketType.ReadResponse;

        protected override void Load(BinaryReader reader)
        {
            var pointsCount = reader.ReadInt32();

            Points = Enumerable.Range(0, pointsCount)
                .Select(x => new DataPoint(reader.ReadUInt64(), reader.ReadSingle()))
                .ToList();
        }

        protected override void Save(BinaryWriter writer)
        {
            writer.Write(Points.Count);

            foreach (var point in Points)
            {
                writer.Write(point.Timestamp);
                writer.Write(point.Value);
            }
        }
    }
}