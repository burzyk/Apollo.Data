namespace ShakaDB.Client.Tests
{
    using System;
    using System.Linq;
    using Xunit;

    public class SessionTests
    {
        [Fact]
        public void ServerRunningTest()
        {
            using (var session = ShakaDbSession.Open("localhost", 8487))
            {
                session.Truncate(TestConstants.UsdAud);
            }
        }

        [Fact]
        public void InvalidServerTest()
        {
            Assert.Throws<ShakaDbException>(() => ShakaDbSession.Open("blah.blah", 998));
        }

        [Fact]
        public void ReuseClosedSessionTest()
        {
            var s = ShakaDbSession.Open("localhost", 8487);
            s.Dispose();

            Assert.Throws<ObjectDisposedException>(() => s.Read(0, 0, 0));
            Assert.Throws<ObjectDisposedException>(() => s.Write(0, null));
            Assert.Throws<ObjectDisposedException>(() => s.Truncate(0));
            Assert.Throws<ObjectDisposedException>(() => s.Dispose());
        }

        [Fact]
        public void ServerReadAndWriteIteatorTest()
        {
            using (var session = ShakaDbSession.Open("localhost", 8487))
            {
                session.Truncate(TestConstants.UsdAud);
                session.Write(TestConstants.UsdAud, new[]
                {
                    new ShakaDbDataPoint(1, 12),
                    new ShakaDbDataPoint(2, 13)
                });

                var result = session.Read(TestConstants.UsdAud).ToList();

                Assert.Equal(2, result.Count);
                Assert.Equal(1, (int) result[0].Timestamp);
                Assert.Equal(2, (int) result[1].Timestamp);
                Assert.Equal(12, result[0].Value);
                Assert.Equal(13, (int) result[1].Value);
            }
        }

        [Fact]
        public void ServerReadAndWriteWithIteatorAndLimitTest()
        {
            using (var session = ShakaDbSession.Open("localhost", 8487))
            {
                session.Truncate(TestConstants.UsdAud);
                session.Write(TestConstants.UsdAud, new[]
                {
                    new ShakaDbDataPoint(1, 12),
                    new ShakaDbDataPoint(2, 13)
                });

                var result = session.Read(TestConstants.UsdAud, 0, 2).ToList();

                Assert.Equal(1, result.Count);
                Assert.Equal(1, (int) result[0].Timestamp);
                Assert.Equal(12, result[0].Value);
            }
        }

        [Fact]
        public void ServerReadAndWriteWithIteatorAndPointsPerPacketLimitTest()
        {
            using (var session = ShakaDbSession.Open("localhost", 8487))
            {
                session.Truncate(TestConstants.UsdAud);
                session.Write(TestConstants.UsdAud, new[]
                {
                    new ShakaDbDataPoint(1, 12),
                    new ShakaDbDataPoint(2, 13),
                    new ShakaDbDataPoint(3, 15)
                });

                var result = session.Read(TestConstants.UsdAud, pointsPerPacket: 2).ToList();

                Assert.Equal(3, result.Count);
            }
        }

        [Fact]
        public void ReadLatestNoDataTest()
        {
            using (var session = ShakaDbSession.Open("localhost", 8487))
            {
                session.Truncate(TestConstants.UsdAud);
                var latest = session.GetLatest(TestConstants.UsdAud);

                Assert.Null(latest);
            }
        }

        [Fact]
        public void ReadLatestTest()
        {
            using (var session = ShakaDbSession.Open("localhost", 8487))
            {
                session.Truncate(TestConstants.UsdAud);
                session.Write(TestConstants.UsdAud, new[]
                {
                    new ShakaDbDataPoint(1, 12),
                    new ShakaDbDataPoint(2, 13),
                    new ShakaDbDataPoint(3, 15)
                });

                var latest = session.GetLatest(TestConstants.UsdAud);

                Assert.Equal(3, (int) latest.Timestamp);
                Assert.Equal(15, latest.Value);
            }
        }

        [Fact]
        public void ServerTruncateTest()
        {
            using (var session = ShakaDbSession.Open("localhost", 8487))
            {
                session.Truncate(TestConstants.UsdAud);
                session.Write(TestConstants.UsdAud, new[]
                {
                    new ShakaDbDataPoint(1, 12),
                    new ShakaDbDataPoint(2, 13)
                });

                var result = session.Read(TestConstants.UsdAud, 0, 2).ToList();

                Assert.Equal(1, result.Count);
                Assert.Equal(1, (int) result[0].Timestamp);
                Assert.Equal(12, result[0].Value);

                session.Truncate(TestConstants.UsdAud);

                result = session.Read(TestConstants.UsdAud).ToList();
                Assert.Equal(0, result.Count);
            }
        }

        [Fact]
        public void MultipleOpenedReadsTest()
        {
            using (var session = ShakaDbSession.Open("localhost", 8487))
            {
                session.Truncate(TestConstants.UsdAud);
                session.Write(TestConstants.UsdAud, new[]
                {
                    new ShakaDbDataPoint(1, 12),
                    new ShakaDbDataPoint(2, 13)
                });

                session.Read(TestConstants.UsdAud);
                Assert.Throws<ShakaDbException>(() => session.Read(TestConstants.UsdAud));
            }
        }
    }
}