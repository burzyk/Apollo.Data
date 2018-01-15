namespace ShakaDB.Client.Protocol
{
    using System.IO;

    public class TruncateRequest : BasePacket
    {
        public TruncateRequest(uint dataSeriesId)
            : base(PacketType.TruncateRequest)
        {
            DataSeriesId = dataSeriesId;
        }

        public TruncateRequest(byte[] payload)
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