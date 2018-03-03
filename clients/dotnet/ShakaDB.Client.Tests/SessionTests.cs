namespace ShakaDB.Client.Tests
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Threading.Tasks;
    using Xunit;

    public class SessionTests
    {
        public const int UsdAud = 1;

        [Fact]
        public async Task ServerRunningTest()
        {
            using (var session = await ShakaDbSession.Open("localhost", 8487))
            {
                await session.Truncate(UsdAud);
            }
        }

        [Fact]
        public async Task ReuseClosedSessionTest()
        {
            var s = await ShakaDbSession.Open("localhost", 8487);
            s.Dispose();

            await Assert.ThrowsAsync<ObjectDisposedException>(async () => await s.Read(0, 0, 0));
            await Assert.ThrowsAsync<ObjectDisposedException>(async () => await s.Write(0, new List<DataPoint>()));
            await Assert.ThrowsAsync<ObjectDisposedException>(async () => await s.Truncate(0));
        }


        [Fact]
        public async Task ServerReadAndWriteIteatorTest()
        {
            using (var session = await ShakaDbSession.Open("localhost", 8487))
            {
                await session.Truncate(UsdAud);
                await session.Write(UsdAud, new[]
                {
                    new DataPoint(1, 12),
                    new DataPoint(2, 13)
                });

                var result = (await session.Read(UsdAud)).ToList();

                Assert.Equal(2, result.Count);
                Assert.Equal(1, (int) result[0].Timestamp);
                Assert.Equal(2, (int) result[1].Timestamp);
                Assert.Equal(12, result[0].ValueAsFloat);
                Assert.Equal(13, (int) result[1].ValueAsFloat);
            }
        }

        [Fact]
        public async Task ServerReadAndWriteWithIteatorAndLimitTest()
        {
            using (var session = await ShakaDbSession.Open("localhost", 8487))
            {
                await session.Truncate(UsdAud);
                await session.Write(UsdAud, new[]
                {
                    new DataPoint(1, 12),
                    new DataPoint(2, 13)
                });

                var result = (await session.Read(UsdAud, 0, 2)).ToList();

                Assert.Equal(1, result.Count);
                Assert.Equal(1, (int) result[0].Timestamp);
                Assert.Equal(12, result[0].ValueAsFloat);
            }
        }

        [Fact]
        public async Task ReadLatestNoDataTest()
        {
            using (var session = await ShakaDbSession.Open("localhost", 8487))
            {
                await session.Truncate(UsdAud);
                var latest = await session.GetLatest(UsdAud);

                Assert.Null(latest);
            }
        }

        [Fact]
        public async Task ReadLatestTest()
        {
            using (var session = await ShakaDbSession.Open("localhost", 8487))
            {
                await session.Truncate(UsdAud);
                await session.Write(UsdAud, new[]
                {
                    new DataPoint(1, 12),
                    new DataPoint(2, 13),
                    new DataPoint(3, 15)
                });

                var latest = await session.GetLatest(UsdAud);

                Assert.Equal(3, (int) latest.Timestamp);
                Assert.Equal(15, latest.ValueAsFloat);
            }
        }

        [Fact]
        public async Task ServerTruncateTest()
        {
            using (var session = await ShakaDbSession.Open("localhost", 8487))
            {
                await session.Truncate(UsdAud);
                await session.Write(UsdAud, new[]
                {
                    new DataPoint(1, 12),
                    new DataPoint(2, 13)
                });

                var result = (await session.Read(UsdAud, 0, 2)).ToList();

                Assert.Equal(1, result.Count);
                Assert.Equal(1, (int) result[0].Timestamp);
                Assert.Equal(12, result[0].ValueAsFloat);

                await session.Truncate(UsdAud);

                result = (await session.Read(UsdAud)).ToList();
                Assert.Equal(0, result.Count);
            }
        }

        [Fact]
        public async Task MultipleOpenedReadsTest()
        {
            using (var session = await ShakaDbSession.Open("localhost", 8487))
            {
                await session.Truncate(UsdAud);
                await session.Write(UsdAud, new[]
                {
                    new DataPoint(1, 12),
                    new DataPoint(2, 13)
                });

                var read = await session.Read(UsdAud);
                await Assert.ThrowsAsync<ShakaDbException>(async () => await session.Read(UsdAud));

                read.ToList();
                (await session.Read(UsdAud)).ToList();
            }
        }

        [Fact]
        public async Task ServerWriteVariableSizeTest()
        {
            using (var session = await ShakaDbSession.Open("localhost", 8487))
            {
                await session.Truncate(UsdAud);
                await session.Write(UsdAud, new[]
                {
                    new DataPoint(new DateTimeOffset(new DateTime(2012, 1, 1)), "ala ma kota"),
                    new DataPoint(new DateTimeOffset(new DateTime(2013, 1, 1)), "ola ma asa"),
                    new DataPoint(new DateTimeOffset(new DateTime(2014, 1, 1)), "kamis ma rica")
                }, 100);

                var result = (await session.Read(UsdAud)).ToList();

                Assert.Equal(3, result.Count);
                Assert.Equal("ala ma kota", result[0].ValueAsString);
                Assert.Equal("ola ma asa", result[1].ValueAsString);
                Assert.Equal("kamis ma rica", result[2].ValueAsString);
            }
        }

        [Fact]
        public async Task ServerWriteMultipleBatchesTest()
        {
            using (var session = await ShakaDbSession.Open("localhost", 8487))
            {
                await session.Truncate(UsdAud);

                var expectedDataCount = Constants.MaxPointsPerPacket * 2 + 1;
                var data = Enumerable.Range(1, expectedDataCount)
                    .Select(x => new DataPoint((ulong) x, x + 1000f))
                    .ToList();

                await session.Write(UsdAud, data);

                var result = (await session.Read(UsdAud)).ToList();

                Assert.True(data.SequenceEqual(result));
            }
        }
    }
}