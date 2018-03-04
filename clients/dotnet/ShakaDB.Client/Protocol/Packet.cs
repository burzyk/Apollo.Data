namespace ShakaDB.Client.Protocol
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;

    public static class Packet
    {
        public static byte[] WriteRequest(uint seriesId, List<DataPoint> points, int? pointSize = null)
        {
            if (points.Select(x => x.Value.Length).Distinct().Count() > 1 && pointSize == null)
            {
                throw new InvalidOperationException("PointSize needs to be specified if values have different size");
            }

            var ms = new MemoryStream();

            using (var w = new BinaryWriter(ms))
            {
                w.Write((byte) PacketType.WriteRequest);
                w.Write(seriesId);
                w.Write((ulong) points.Count);
                w.Write((uint) (pointSize ?? (points.Any() ? points[0].Value.Length + 8 : 0)));

                foreach (var point in points)
                {
                    var writeBuffer = pointSize != null ? new byte[pointSize.Value - 8] : null;
                    var toWrite = point.Value;

                    if (writeBuffer != null)
                    {
                        // ensure the size of the buffer is alligned
                        Array.Copy(point.Value, writeBuffer, point.Value.Length);
                        toWrite = writeBuffer;
                    }

                    w.Write(point.Timestamp);
                    w.Write(toWrite);
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