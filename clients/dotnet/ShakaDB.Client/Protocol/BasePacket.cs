namespace ShakaDB.Client.Protocol
{
    using System.IO;

    public abstract class BasePacket
    {
        protected BasePacket()
        {
        }

        protected BasePacket(byte[] payload)
        {
            using (var r = new BinaryReader(new MemoryStream(payload)))
            {
                var type = (PacketType) r.ReadByte();

                if (type != PacketType)
                {
                    throw new ShakaDbException("Received unexpected packet type");
                }

                Load(r);
            }
        }

        public abstract PacketType PacketType { get; }

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