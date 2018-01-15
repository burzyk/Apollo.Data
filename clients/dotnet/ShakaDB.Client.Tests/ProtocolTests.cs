namespace ShakaDB.Client.Tests
{
    using Protocol;
    using Xunit;

    public class ProtocolTests
    {
        [Fact]
        public void SerializeWriteRequestTest()
        {
            var points = new[]
            {
                new DataPoint(1, 2.4f),
                new DataPoint(1, 5.4f)
            };
            var request = new WriteRequest(13, points);

            var serialized = request.Serialize();

            Assert.Equal(33, serialized.Length);

            var deserialized = new WriteRequest(serialized);

            Assert.Equal(PacketType.WriteRequest, deserialized.PacketType);
            Assert.Equal(2, deserialized.Points.Count);
            Assert.Equal(13, (int) deserialized.DataSeriesId);
            Assert.Equal(1, (int) deserialized.Points[0].Timestamp);
            Assert.Equal(2.4f, deserialized.Points[0].Value);
            Assert.Equal(1, (int) deserialized.Points[1].Timestamp);
            Assert.Equal(5.4f, deserialized.Points[1].Value);
        }

        [Fact]
        public void SerializeReadRequestTest()
        {
            var request = new ReadRequest(15, 0, 100, 123);

            var serialized = request.Serialize();

            Assert.Equal(25, serialized.Length);

            var deserialized = new ReadRequest(serialized);

            Assert.Equal(PacketType.ReadRequest, deserialized.PacketType);
            Assert.Equal(15, (int) deserialized.DataSeriesId);
            Assert.Equal(0L, (long) deserialized.Begin);
            Assert.Equal(100L, (long) deserialized.End);
            Assert.Equal(123, deserialized.PointsPerPacket);
        }
    }
}