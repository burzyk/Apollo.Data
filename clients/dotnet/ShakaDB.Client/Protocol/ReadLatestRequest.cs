namespace ShakaDB.Client.Protocol
{
    using System.IO;

    public class ReadLatestRequest : BasePacket
    {
        public ReadLatestRequest(uint dataSeriesId)
            : base(PacketType.ReadLatestRequest)
        {
            DataSeriesId = dataSeriesId;
        }

        public ReadLatestRequest(byte[] payload)
            : base(payload)
        {
        }

        public uint DataSeriesId { get; private set; }

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