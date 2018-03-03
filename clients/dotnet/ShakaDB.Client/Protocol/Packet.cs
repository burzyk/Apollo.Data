namespace ShakaDB.Client.Protocol
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;

    public static class Packet
    {
        public static byte[] WriteRequest(uint seriesId, List<DataPoint> points)
        {
            var ms = new MemoryStream();

            using (var w = new BinaryWriter(ms))
            {
                w.Write((byte) PacketType.WriteRequest);
                w.Write(seriesId);
                w.Write((ulong) points.Count);
                w.Write((uint) (points.Any() ? points[0].Value.Length + 8 : 0));

                foreach (var point in points)
                {
                    w.Write(point.Timestamp);
                    w.Write(point.Value);
                }
            }

            return ms.ToArray();
        }

        public static byte[] ReadRequest(uint seriesId, ulong begin, ulong end)
        {
            var ms = new MemoryStream();

            using (var w = new BinaryWriter(ms))
            {
                w.Write((byte) PacketType.ReadRequest);
                w.Write(seriesId);
                w.Write(begin);
                w.Write(end);
            }

            return ms.ToArray();
        }

        public static byte[] ReadLatestRequest(uint seriesId)
        {
            var ms = new MemoryStream();

            using (var w = new BinaryWriter(ms))
            {
                w.Write((byte) PacketType.ReadLatestRequest);
                w.Write(seriesId);
            }

            return ms.ToArray();
        }

        public static byte[] TruncateRequest(uint seriesId)
        {
            var ms = new MemoryStream();

            using (var w = new BinaryWriter(ms))
            {
                w.Write((byte) PacketType.TruncateRequest);
                w.Write(seriesId);
            }

            return ms.ToArray();
        }

        public static List<DataPoint> ReadResponse(byte[]response)
        {
            using (var r = new BinaryReader(new MemoryStream(response)))
            {
                var result = new List<DataPoint>();

                var type = (PacketType) r.ReadByte();

                if (type != PacketType.ReadResponse)
                {
                    throw new InvalidOperationException("Invalid packet type");
                }

                var pointsCount = r.ReadUInt64();
                var pointSize = r.ReadUInt32();

                for (ulong i = 0; i < pointsCount; i++)
                {
                    result.Add(new DataPoint(r.ReadUInt64(), r.ReadBytes((int) pointSize - 8)));
                }

                return result;
            }
        }

        public static ResponseCode ReadSimpleResponse(byte[]response)
        {
            using (var r = new BinaryReader(new MemoryStream(response)))
            {
                var type = (PacketType) r.ReadByte();

                if (type != PacketType.SimpleResponse)
                {
                    throw new InvalidOperationException("Invalid packet type");
                }

                return (ResponseCode) r.ReadByte();
            }
        }
    }
}