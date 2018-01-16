namespace ShakaDB.Client.Protocol
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Runtime.InteropServices;

    public class ReadRequest : BasePacket
    {
        public ReadRequest(uint dataSeriesId, ulong begin, ulong end, int pointsPerPacket)
        {
            DataSeriesId = dataSeriesId;
            Begin = begin;
            End = end;
            PointsPerPacket = pointsPerPacket;
        }

        public ReadRequest(byte[] payload) : base(payload)
        {
        }

        public uint DataSeriesId { get; private set; }

        public ulong Begin { get; private set; }

        public ulong End { get; private set; }

        public int PointsPerPacket { get; private set; }

        public override PacketType PacketType => PacketType.ReadRequest;

        protected override void Load(BinaryReader reader)
        {
            DataSeriesId = reader.ReadUInt32();
            Begin = reader.ReadUInt64();
            End = reader.ReadUInt64();
            PointsPerPacket = reader.ReadInt32();
        }

        protected override void Save(BinaryWriter writer)
        {
            writer.Write(DataSeriesId);
            writer.Write(Begin);
            writer.Write(End);
            writer.Write(PointsPerPacket);
        }
    }
}