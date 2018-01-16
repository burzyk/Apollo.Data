namespace ShakaDB.Client.Protocol
{
    using System.IO;

    public class ReadLatestRequest : BasePacket
    {
        public ReadLatestRequest(uint dataSeriesId)
        {
            DataSeriesId = dataSeriesId;
        }

        public ReadLatestRequest(byte[] payload)
            : base(payload)
        {
        }

        public uint DataSeriesId { get; private set; }

        public override PacketType PacketType => PacketType.ReadLatestRequest;

        protected override void Load(BinaryReader reader)
        {
            DataSeriesId = reader.ReadUInt32();
        }

        protected override void Save(BinaryWriter writer)
        {
            writer.Write(DataSeriesId);
        }
    }
}