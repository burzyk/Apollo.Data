namespace ShakaDB.Client.Protocol
{
    using System.IO;

    public abstract class BasePacket
    {
        protected BasePacket(PacketType packetType)
        {
            PacketType = packetType;
        }

        protected BasePacket(byte[] payload)
        {
            using (var r = new BinaryReader(new MemoryStream(payload)))
            {
                PacketType = (PacketType) r.ReadByte();

                Load(r);
            }
        }

        public PacketType PacketType { get; private set; }

        public byte[] Serialize()
        {
            var ms = new MemoryStream();
            var writer = new BinaryWriter(ms);

            writer.Write((byte) PacketType);

            Save(writer);

            return ms.ToArray();
        }

        protected abstract void Load(BinaryReader reader);

        protected abstract void Save(BinaryWriter writer);
    }
}