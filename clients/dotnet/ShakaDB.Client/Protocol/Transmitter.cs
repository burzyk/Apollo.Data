namespace ShakaDB.Client.Protocol
{
    using System.IO;
    using System.Net.Sockets;
    using System.Text;
    using System.Threading.Tasks;

    public static class Transmitter
    {
        private const uint Magic = 0x4B414D41;
        private const int HeaderSize = 8;

        public static async Task<byte[]> Receive(Stream input)
        {
            var header = await ReadBuffer(input, HeaderSize);

            using (var r = new BinaryReader(new MemoryStream(header), Encoding.ASCII, true))
            {
                var magic = r.ReadUInt32();
                var totalSize = r.ReadInt32();

                if (magic != Magic)
                {
                    throw new ShakaDbException("Received malformed packet");
                }

                return await ReadBuffer(input, totalSize - HeaderSize);
            }
        }

        public static async Task Send(byte[] payload, Stream output)
        {
            var header = new MemoryStream();

            using (var w = new BinaryWriter(header))
            {
                w.Write(Magic);
                w.Write(payload.Length + HeaderSize);

                await output.WriteAsync(header.ToArray(), 0, HeaderSize);
                await output.WriteAsync(payload, 0, payload.Length);
            }
        }

        private static async Task<byte[]> ReadBuffer(Stream stream, int count)
        {
            var result = new byte[count];
            var read = 0;
            var totalRead = 0;

            while ((read = await stream.ReadAsync(result, totalRead, count - totalRead)) > 0)
            {
                totalRead += read;
            }

            if (totalRead != count)
            {
                throw new ShakaDbException("Failed to read all bytes");
            }

            return result;
        }
    }
}